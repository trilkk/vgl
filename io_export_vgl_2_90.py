#!BPY

bl_info = {
    "name": "Export VGL header data (.hpp)",
    "author": "Faemiyah",
    "version": (1, 0, 0),
    "blender": (2, 82, 0),
    "location": "File > Export > VGL C++ header data (.hpp)",
    "description": "Export VGL C++ header data (.hpp)",
    "warning": "",
    "category": "Import-Export"}

"""
VGL C++ header export script and animation panel.
http://faemiyah.fi/demoscene/vgl
"""

import bpy
import copy
import os
import re
from mathutils import Vector

# TODO: Remove useless imports
from bpy.props import (
        StringProperty,
        BoolProperty,
        CollectionProperty,
        EnumProperty,
        FloatProperty,
        )
from bpy_extras.io_utils import (
        ExportHelper,
        )
from bpy.types import (
        Operator,
        Panel
        )

########################################
# Misc. ################################
########################################

def is_verbose():
    """Verbose debug mode toggle function."""
    return False

def vgl_log(op):
    """Logging function that identifies the script."""
    print("io_export_vgl.py: " + op)

########################################
# Templates ############################
########################################

class Template:
    """Class for templated string generation."""

    def __init__(self, content):
        """Constructor."""
        self.__content = content

    def format(self, substitutions=None):
        """Return formatted output."""
        ret = self.__content
        if substitutions:
            for kk in substitutions:
                vv = substitutions[kk].replace("\\", "\\\\")
                (ret, num) = re.subn(r'\[\[\s*%s\s*\]\]' % (kk), vv, ret)
                if not num:
                    print("WARNING: substitution '%s' has no matches" % (kk))
        unmatched = list(set(re.findall(r'\[\[([^\]]+)\]\]', ret)))
        (ret, num) = re.subn(r'\[\[[^\]]+\]\]', "", ret)
        if num and is_verbose():
            print("Template substitutions not matched: %s (%i)" % (str(unmatched), num))
        return ret

    def __str__(self):
        """String representation."""
        return self.__content

g_template_header = Template("""#ifndef [[HEADER_NAME]]
#define [[HEADER_NAME]]\n
[[MODEL_DATA]]\n
#endif""")

g_template_mesh = Template("""int16_t g_vertices_[[MODEL_NAME]][] =
{
[[VERTEX_DATA]]
};\n
[[INDEX_TYPE]] g_indices_[[MODEL_NAME]][] =
{
[[INDEX_DATA]]
};""")

g_template_mesh = Template("""const unsigned g_vertices_[[MODEL_NAME]]_size = [[VERTEX_DATA_SIZE]];\n
const [[VERTEX_DATA_TYPE]] g_vertices_[[MODEL_NAME]][] =
{
[[VERTEX_DATA]]
};\n
const unsigned g_indices_[[MODEL_NAME]]_size = [[INDEX_DATA_SIZE]];\n
const [[INDEX_DATA_TYPE]] g_indices_[[MODEL_NAME]][] =
{
[[INDEX_DATA]]
};""")

g_template_weights = Template("""const [[WEIGHT_DATA_TYPE]] g_weights_[[MODEL_NAME]][] =
{
[[WEIGHT_DATA]]
};""")

g_template_armature = Template("""const unsigned g_bones_[[MODEL_NAME]]_size = [[BONE_DATA_SIZE]];\n
const [[BONE_DATA_TYPE]] g_bones_[[MODEL_NAME]][] =
{
[[BONE_DATA]]
};\n
const unsigned g_armature_[[MODEL_NAME]]_size = [[ARMATURE_DATA_SIZE]];\n
const [[ARMATURE_DATA_TYPE]] g_armature_[[MODEL_NAME]][] =
{
[[ARMATURE_DATA]]
};""")

g_template_anim = Template("""const unsigned g_animation_[[MODEL_NAME]]_[[ANIM_NAME]]_size = [[ANIM_DATA_SIZE]];\n
const [[ANIM_DATA_TYPE]] g_animation_[[MODEL_NAME]]_[[ANIM_NAME]][] =
{
[[ANIM_DATA]]
};""")

g_indent = "  "

########################################
# Pose #################################
########################################

class Pose:
    """Abstraction of pose."""

    def __init__(self, pose, index):
        """Constructor."""
        self.__pose = pose
        self.__index = index
        match = re.match(r'\s*(\S+)\s+([\d\.]+)\s*$', pose.name.strip())
        if match:
            self.__basename = match.groups(1)[0]
            self.__time = float(match.groups(1)[1])
        else:
            self.__basename = pose.name
            self.__time = 0.0
            vgl_log("WARNING: cannot deduce timestamp from pose name '%s', using %f" % (self.__basename, self.__time))

    def getIndex(self):
        """Accessor."""
        return self.__index

    def getName(self):
        """Accessor."""
        return self.__basename

    def getTime(self):
        """Accessor."""
        return self.__time

    def __lt__(self, other):
        """Less than function."""
        return self.__time < other.getTime()

########################################
# Export ###############################
########################################

def isExportName(name):
    """Tell if named object wants to be exported."""
    return name.startswith("g_")

def toExportName(name):
    """Convert name to .cpp -friendly name."""
    ret = re.sub(r'\.', r'_', name)
    return re.sub(r'\s', r'_', ret)

def toExport8F8(op):
    """Converts number to exportable 8.8 signed fixed point number."""
    return int(round(op * 256.0))

def toExport4F12(op):
    """Converts number to exportable 4.12 signed fixed point number."""
    return int(round(op * 4096.0))

def toExportS16(op):
    """Converts number to exportable signed 16-bit number."""
    return max(min(int(round(op)), 32767), -32768)

def toExportU8(op):
    """Converts number to exportable unsigned 8-bit number."""
    return max(min(int(round(op)), 255), 0)

def toExportString(lst, data_type):
    """Converts input data to exported string."""
    ret = ""
    total_count = 0
    if not isinstance(lst, list):
        raise RuntimeError("input is not list: %s" % (str(lst)))
    for ii in lst:
        if not isinstance(ii, list):
            raise RuntimeError("input element is not list: %s" % (str(ii)))
        if ret:
            ret += "\n"
        ret += g_indent + ", ".join(map(lambda x: str(x), ii)) + ","
        total_count += len(ii)
    if data_type in ["uint8_t", "int8_t"]:
        remainder = 4 - (total_count % 4)
        if remainder == 4:
            remainder = 0
    elif data_type in ["uint16_t", "int16_t"]:
        remainder = (total_count % 2)
    elif data_type in ["uint32_t", "int32_t", "uint64_t", "int64_t", "float", "double"]:
        remainder = 0
    else:
        raise RuntimeError("unknown data type: %s" % (data_type))
    if remainder != 0:
        ret += "\n#if defined(__x86_64__) || defined(__i386__)\n" + g_indent + ", ".join(["0"] * remainder) + ",\n#endif"
    return ret

def findRootBone(bone):
    """Find bone root."""
    if not bone.parent:
        return bone
    return findRootBone(bone.parent)

def toExportBoneQuaternion(bone_orig, bone_curr):
    """Create exportable bone oritentation by undoing Blender's bone transform mangling."""
    world = bone_orig.matrix_local.inverted()
    qq = (bone_curr.matrix @ world).to_quaternion()
    return qq

def toExportBonePosition(mat, bone_quaternion, bone_orig, bone_curr):
    """Create exportable bone position by creating a difference transform from neutral state."""
    pos_orig = mat @ bone_orig.head_local
    pos_curr = mat @ bone_curr.head
    # If original position is neutralized later, can simply return current position as-is.
    return (bone_quaternion.to_matrix() @ (-pos_orig)) + pos_curr

def meshFindMaxVertexValue(msh, mesh_scale):
    """Finds greatest vertex value in mesh."""
    ret = 0.0
    for ii in msh.vertices:
        px = abs(ii.co[0] * mesh_scale[0])
        py = abs(ii.co[1] * mesh_scale[1])
        pz = abs(ii.co[2] * mesh_scale[2])
        ret = max(max(max(px, py), pz), ret)
    return ret

def meshGetVertexData(msh, mesh_scale, export_scale):
    """Gets vertex data from a mesh."""
    ret = []
    for ii in msh.vertices:
        px = toExportS16(ii.co[0] * mesh_scale[0] * export_scale)
        py = toExportS16(ii.co[1] * mesh_scale[1] * export_scale)
        pz = toExportS16(ii.co[2] * mesh_scale[2] * export_scale)
        ret += [[px, py, pz]]
    return ret

def meshIndexDataToTriangle(msh, face, idx1, idx2, idx3):
    """Create string for one triangle with given vertices."""
    vv = face.vertices
    if (len(msh.materials) > 0) and msh.materials[face.material_index] and bpy.context.scene.vgl_export_color:
        color = msh.materials[face.material_index].diffuse_color
        cr = toExportU8(color[0] * 255.0)
        cg = toExportU8(color[1] * 255.0)
        cb = toExportU8(color[2] * 255.0)
        return [vv[idx1], vv[idx2], vv[idx3], cr, cg, cb]
    return [vv[idx1], vv[idx2], vv[idx3]]

def meshGetTriangleData(msh):
    """Gets index data as triangle list from a mesh."""
    ret = []
    for ii in msh.polygons:
        # Fan out all higher-order polygons.
        for jj in range(2, len(ii.vertices)):
            ret += [meshIndexDataToTriangle(msh, ii, 0, jj - 1, jj)]
    return ret

def normalizedWeightData(data, vmap):
    """Turns sorted weight data block into a normalized mode."""
    # First convert existing references before sorting.
    for ii in data:
        ii[1] = vmap[ii[1]]
    data = sorted(data, key = lambda x: x[0], reverse = True)
    # Ensure data length is exactly 3.
    while 3 > len(data):
        data += [(0.0, 0)]
    data = data[:3]
    # Calculate total weights.
    total_weight = 0.0
    for ii in data:
        total_weight += ii[0]
    w1 = toExportU8(data[0][0] * 255.0 / total_weight)
    w2 = toExportU8(data[1][0] * 255.0 / total_weight)
    w3 = toExportU8(data[2][0] * 255.0 / total_weight)
    return [w1, w2, w3, data[0][1], data[1][1], data[2][1]]

def meshGetWeightData(msh, vmap):
    """Gets weight data from a mesh."""
    ret = []
    for ii in msh.vertices:
        grp = []
        for jj in ii.groups:
            # Check for aberrations.
            if 0.0 > jj.weight:
                raise RuntimeError("invalid vertex weight: %f" % (jj.weight))
            grp += [[jj.weight, jj.group]]
        ret += [normalizedWeightData(grp, vmap)]
    return ret

def meshToString(context, msh, vmap, name, mesh_scale, export_scale):
    """Returns C++ code string from a mesh."""
    vdata = meshGetVertexData(msh, mesh_scale, export_scale)
    idata = meshGetTriangleData(msh)
    subst = {
            "MODEL_NAME" : name,
            "VERTEX_DATA" : toExportString(vdata, "int16_t"),
            "VERTEX_DATA_TYPE" : "int16_t",
            "VERTEX_DATA_SIZE" : str(len(vdata)),
            "INDEX_DATA_SIZE" : str(len(idata)),
            }
    # Index type depends on the number of indices.
    if len(msh.vertices) >= 65536:
        subst["INDEX_DATA"] = toExportString(idata, "uint32_t")
        subst["INDEX_DATA_TYPE"] = "uint32_t"
    else:
        subst["INDEX_DATA"] = toExportString(idata, "uint16_t")
        subst["INDEX_DATA_TYPE"] = "uint16_t"
    ret = g_template_mesh.format(subst)
    # May need to add weight data data for meshes with armatures.
    if vmap:
        wdata = meshGetWeightData(msh, vmap)
        if wdata:
            if len(wdata) != len(vdata):
                raise RuntimeError("weight data size (%i) does not match vertex data size (%i)" % (len(wdata), len(vdata)))
            subst = {
                    "MODEL_NAME" : name,
                    "WEIGHT_DATA" : toExportString(wdata, "uint8_t"),
                    "WEIGHT_DATA_TYPE" : "uint8_t",
                    }
            ret += "\n\n" + g_template_weights.format(subst)
    return ret

def armatureGetBoneData(arm, mat, armature_scale, export_scale):
    """Gets bone data from an armature."""
    ret = []
    for ii in arm.bones:
        hd = mat @ ii.head_local
        hd0 = toExportS16(hd[0] * armature_scale[0] * export_scale)
        hd1 = toExportS16(hd[1] * armature_scale[1] * export_scale)
        hd2 = toExportS16(hd[2] * armature_scale[2] * export_scale)
        ret += [[hd0, hd1, hd2]]
    return ret

def findBoneIndex(arm, bone):
    """Find index of given bone in an armature."""
    for ii in range(len(arm.bones)):
        if arm.bones[ii] == bone:
            return ii
    return -1

def armatureToString(arm, name, mat, armature_scale, export_scale):
    """Returns C++ code string from an armature."""
    bdata = armatureGetBoneData(arm, mat, armature_scale, export_scale)
    # Armature data.
    adata = []
    armature_data_size = 0
    for ii in arm.bones:
        child_list = [len(ii.children)]
        armature_data_size += 1
        for jj in ii.children:
            idx = findBoneIndex(arm, jj)
            if 0 > idx:
                raise RuntimeError("could not locate bone '%s' index" % (jj.name))
            child_list += [idx]
        adata += [child_list]
        armature_data_size += len(child_list)
    subst = {
            "MODEL_NAME" : name,
            "BONE_DATA" : toExportString(bdata, "int16_t"),
            "BONE_DATA_SIZE" : str(len(bdata)),
            "BONE_DATA_TYPE" : "int16_t",
            "ARMATURE_DATA" : toExportString(adata, "uint8_t"),
            "ARMATURE_DATA_SIZE" : str(armature_data_size),
            "ARMATURE_DATA_TYPE" : "uint8_t",
            }
    return g_template_armature.format(subst)

def getArmatureOrdering(arm):
    """Get mapping from armature names to indices."""
    ret = []
    for ii in arm.bones:
        ret += [ii.name]
    return ret

def getVertexGroupOrdering(msh, amap):
    """Get vertex group transform for given mesh and armature map."""
    ret = {}
    for ii in range(len(msh.vertex_groups)):
        grp = msh.vertex_groups[ii]
        ret[ii] = amap.index(grp.name)
    return ret

def findArmature(context):
    """Finds the first armature in a context."""
    for ii in context.selectable_objects:
        if (ii.type == "ARMATURE"):
            # Instance as opposed to original.
            if ii.users == 1:
                return ii
    return None

def findMesh(context):
    """Finds the first mesh in a context."""
    ret = None
    for ii in context.selectable_objects:
        if (ii.type == "MESH"):
            # No idea how to select original as opposed to instance :(
            if ii.users >= 1:
                ret = ii
    return ret

def collectPoses(pl):
    """Collects poses from a pose library."""
    ret = {}
    for ii in range(0, len(pl.pose_markers)):
        pose = Pose(pl.pose_markers[ii], ii)
        basename = pose.getName()
        if not basename in ret:
            ret[basename] = []
        ret[basename] += [pose]
    for kk in ret.keys():
        ret[kk] = sorted(ret[kk])
    return ret

def animToString(context, amap, arm, action, model_name, mat, armature_scale, export_scale):
    """Exports singular animation to string."""
    ret = []
    anim_data_size = 0
    # Assign action, iterate to first frame.
    arm.animation_data.action = action
    while True:
        if bpy.ops.screen.keyframe_jump(next=False) != {"FINISHED"}:
            break
        bpy.ops.wm.redraw_timer(type="DRAW_WIN_SWAP", iterations=1)
    # Go forward while saving frames.
    while True:
        # Action must be reassigned or the pose won't update.
        arm.animation_data.action = action
        current_frame = context.scene.frame_current
        elem = [toExport8F8(float(current_frame) / 24.0)]
        anim_data_size += len(elem)
        ret += [elem]
        # Iterate using armature order.
        for jj in amap:
            bone_orig = arm.data.bones[jj]
            bone_curr = arm.pose.bones[jj]
            # Heaven knows why we have to rearrange the quaternion order.
            qq = toExportBoneQuaternion(bone_orig, bone_curr)
            qw = toExport4F12(qq[0])
            qx = toExport4F12(qq[1])
            qy = toExport4F12(qq[2])
            qz = toExport4F12(qq[3])
            # The difference to original position is baked into the animation position.
            hd = toExportBonePosition(mat, qq, bone_orig, bone_curr)
            px = toExportS16(hd[0] * armature_scale[0] * export_scale)
            py = toExportS16(hd[1] * armature_scale[1] * export_scale)
            pz = toExportS16(hd[2] * armature_scale[2] * export_scale)
            elem = [px, py, pz, qw, qx, qy, qz]
            anim_data_size += len(elem)
            ret += [elem]
        # Advance to next frame, abort if not possible.
        if bpy.ops.screen.keyframe_jump(next=True) != {"FINISHED"}:
            break
        bpy.ops.wm.redraw_timer(type="DRAW_WIN_SWAP", iterations=1)
    # Create export string.
    anim_name = toExportName(action.name)
    subst = {
            "MODEL_NAME" : model_name,
            "ANIM_NAME" : anim_name,
            "ANIM_DATA" : toExportString(ret, "int16_t"),
            "ANIM_DATA_SIZE" : str(anim_data_size),
            "ANIM_DATA_TYPE" : "int16_t",
            }
    return g_template_anim.format(subst)

def exportAllMeshesToHeader(filename, context):
    export_strings = []
    # Find mesh.
    msh = findMesh(context)
    if not msh:
        raise RuntimeError("could not find mesh to export")
    export_name = toExportName(msh.name)
    vgl_log("selected mesh for export: '%s' => '%s'" % (msh.name, export_name))
    # Evaluate dependency graph for the mesh, it probably has some decimate or something.
    msh = msh.evaluated_get(context.evaluated_depsgraph_get())
    # Everything needs to be exported in the same scale, discard accuracy and fit in int16_t.
    discard_mul = pow(2.0, float(bpy.context.scene.vgl_export_discard_bits))
    export_scale = 32767.0 / (meshFindMaxVertexValue(msh.data, msh.scale) * discard_mul)
    # Find armatures.
    arm = findArmature(context)
    # Export armature.
    if arm:
        # Not having scale 1 is suspicious.
        if (msh.scale != arm.scale) or (msh.scale != Vector((1.0, 1.0, 1.0))) or (arm.scale != Vector((1.0, 1.0, 1.0))):
            vgl_log("WARNING: suspicious mesh/armature scales: %s ; %s" % (str(msh.scale), str(arm.scale)))
        vgl_log("selected armature for export: '%s'" % (arm.name))
        amap = getArmatureOrdering(arm.data)
        vgl_log("bone mapping: %s" % (str(amap)))
        export_strings += [armatureToString(arm.data, export_name, arm.matrix_basis, arm.scale, export_scale)]
        # Animations in armatures.
        for ii in bpy.data.actions:
            export_strings += [animToString(context, amap, arm, ii, export_name, arm.matrix_basis, arm.scale, export_scale)]
    # Export mesh.
    vmap = None
    if arm:
        vmap = getVertexGroupOrdering(msh, amap)
        vgl_log("vertex mapping: %s" % (str(vmap)))
    export_strings += [meshToString(context, msh.data, vmap, export_name, msh.scale, export_scale)]
    with open(filename, "w") as fd:
        subst = {
                "MODEL_DATA" : "\n\n".join(export_strings),
                "HEADER_NAME" : "__" + os.path.basename(filename).replace(".", "_").lower() + "__",
                }
        fd.write(g_template_header.format(subst))

########################################
# Blender export operator ##############
########################################

bpy.types.Scene.vgl_export_color = bpy.props.BoolProperty(
    name = "vgl_export_color",
    description = "Enable or disable exporting color values for faces",
    default = False)

bpy.types.Scene.vgl_export_discard_bits = bpy.props.IntProperty(
    name = "vgl_export_discard_bits",
    description = "Number of bits to discard before export",
    default = 3)

class VglOperatorExport(Operator, ExportHelper):
    bl_idname = "export.vgl_hpp"
    bl_label = "Export VGL C++ header (.hpp)"
    filename_ext = ".hpp"

    filter_glob: StringProperty(default="*.hpp", options={'HIDDEN'})

    def execute(self, context):
        filu = self.filepath
        exportAllMeshesToHeader(filu, context)
        return {"FINISHED"}

def vgl_menu_export(self, context):
    default_path = os.path.splitext(bpy.data.filepath)[0] + ".hpp"
    self.layout.operator(VglOperatorExport.bl_idname, text="VGL C++ header (.hpp)")

########################################
# Blender integration ##################
########################################

classes = [
    VglOperatorExport,
]

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.TOPBAR_MT_file_export.append(vgl_menu_export)

def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(vgl_menu_export)
    for cls in classes:
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()

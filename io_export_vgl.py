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
VGL C++ header export script.
http://faemiyah.fi/demoscene/vgl
"""

import bpy
import copy
import os
import re
from mathutils import Vector

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
# C++ header generation ################
########################################

class Template:
    """Class for templated string generation."""

    def __init__(self, content):
        """Constructor."""
        self.__content = content
        self.__substitutions = {}

    def format(self):
        """Return formatted output."""
        ret = self.__content
        for kk in self.__substitutions:
            vv = self.__substitutions[kk].replace("\\", "\\\\")
            (ret, num) = re.subn(r'\[\[\s*%s\s*\]\]' % (kk), vv, ret)
            if not num and is_verbose():
                vlg_log("WARNING: substitution '%s' has no matches" % (kk))
        unmatched = list(set(re.findall(r'\[\[([^\]]+)\]\]', ret)))
        (ret, num) = re.subn(r'\[\[[^\]]+\]\]', "", ret)
        if num and is_verbose():
            vgl_log("Template substitutions not matched: %s (%i)" % (str(unmatched), num))
        return ret

    def subst(self, template, replacement):
        """Add a substitution."""
        self.__substitutions[template] = replacement

    def __str__(self):
        """String representation."""
        return self.__content

g_template_header = Template("""#ifndef [[HEADER_NAME]]_HPP
#define [[HEADER_NAME]]_HPP\n
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

g_template_weights = Template("""uint8_t g_weights_[[MODEL_NAME]][] =
{
[[WEIGHT_DATA]]
};""")

g_template_armature = Template("""int16_t g_bones_[[MODEL_NAME]][] =
{
[[BONE_DATA]]
};\n
uint8_t g_armature_[[MODEL_NAME]][] =
{
[[ARMATURE_DATA]]
};""")

g_template_anim = Template("""int16_t g_animation_[[MODEL_NAME]]_[[ANIM_NAME]][] =
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
    match = re.match(r'g_([^\.]+)(\.\d+)?', name)
    if not match:
        raise RuntimeError("name '%s' is not C++ -friendly" % (name))
    return match.groups(1)[0]

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

def findRootBone(bone):
    """Find bone root."""
    if not bone.parent:
        return bone
    return findRootBone(bone.parent)

def toExportQuaternion(bone_orig, bone_curr):
    """Undo Blender's bone transform mangling."""
    #m1 = copy.deepcopy(bone_orig.matrix_local)
    #m2 = copy.deepcopy(bone_curr.matrix)
    #m1.invert()
    #qq = (m1 * m2).to_quaternion()
    #return qq
    world = bone_orig.matrix_local.inverted()
    #parent = findRootBone(bone_orig)
    #world = parent.matrix.inverted().to_4x4
    qq = (bone_curr.matrix @ world).to_quaternion()
    return qq
    #return (world * bone.matrix.to_quaternion().to_matrix()).to_quaternion()
    #qq = bone.rotation_quaternion
    #return (qq[0], -qq[1], -qq[3], qq[2])

def meshFindMaxVertexValue(msh, mesh_scale):
    """Finds greatest vertex value in mesh."""
    ret = 0.0
    for ii in msh.vertices:
        px = abs(ii.co[0] * mesh_scale[0])
        py = abs(ii.co[1] * mesh_scale[1])
        pz = abs(ii.co[2] * mesh_scale[2])
        ret = max(max(max(px, py), pz), ret)
    return ret

def meshVertexDataToString(msh, mesh_scale, export_scale):
    """Converts mesh vertex data to string."""
    ret = []
    for ii in msh.vertices:
        px = toExportS16(ii.co[0] * mesh_scale[0] * export_scale)
        py = toExportS16(ii.co[1] * mesh_scale[1] * export_scale)
        pz = toExportS16(ii.co[2] * mesh_scale[2] * export_scale)
        ret += ["%s%i, %i, %i," % (g_indent, px, py, pz)]
    return "\n".join(ret)

def meshTriangleToString(msh, face, idx1, idx2, idx3):
    """Create string for one triangle with given vertices."""
    vv = face.vertices
    if len(msh.materials) > 0:
        color = msh.materials[face.material_index].diffuse_color
        cr = toExportU8(color[0] * 255.0)
        cg = toExportU8(color[1] * 255.0)
        cb = toExportU8(color[2] * 255.0)
        return "%s%i, %i, %i, %i, %i, %i," % (g_indent, vv[idx1], vv[idx2], vv[idx3], cr, cg, cb)
    return "%s%i, %i, %i," % (g_indent, vv[idx1], vv[idx2], vv[idx3])

def meshIndexDataToString(msh):
    """Converts mesh index data to string."""
    ret = []
    for ii in msh.polygons:
        # Fan out all higher-order polygons.
        for jj in range(2, len(ii.vertices)):
            ret += [meshTriangleToString(msh, ii, 0, jj - 1, jj)]
    return "\n".join(ret)

def normalizedWeightData(data, vmap):
    """Turns sorted weight data block into a normalized mode."""
    # First convert existing references.
    for ii in data:
        ii[1] = vmap[ii[1]]
    # Then add empty stubs before sorting.
    while 3 > len(data):
        data += [(0.0, 0)]
    data = sorted(data, key = lambda x: x[0], reverse = True)
    total_weight = 0.0
    for ii in data:
        total_weight += ii[0]
    w1 = toExportU8(data[0][0] * 255.0 / total_weight)
    w2 = toExportU8(data[1][0] * 255.0 / total_weight)
    w3 = toExportU8(data[2][0] * 255.0 / total_weight)
    return "%s%i, %i, %i, %i, %i, %i," % (g_indent, w1, w2, w3, data[0][1], data[1][1], data[2][1])

def meshWeightDataToString(msh, vmap):
    """Convert mesh group and weight data to strings."""
    ret = []
    for ii in msh.vertices:
        grp = []
        for jj in ii.groups:
            # Check for aberrations.
            if 0.0 > jj.weight:
                raise RuntimeError("invalid vertex weight: %f" % (jj.weight))
            grp += [[jj.weight, jj.group]]
        ret += [normalizedWeightData(grp, vmap)]
    return "\n".join(ret)

def meshToString(msh, vmap, name, mesh_scale, export_scale):
    """Returns C++ code string from a mesh."""
    tmpl = copy.deepcopy(g_template_mesh)
    tmpl.subst("MODEL_NAME", name)
    tmpl.subst("VERTEX_DATA", meshVertexDataToString(msh, mesh_scale, export_scale))
    tmpl.subst("INDEX_TYPE", "unsigned")
    tmpl.subst("INDEX_DATA", meshIndexDataToString(msh))
    # May use smaller index type.
    if len(msh.vertices) < 65536:
        tmpl.subst("INDEX_TYPE", "uint16_t")
    ret = tmpl.format()
    # May need to add group data.
    wdata = meshWeightDataToString(msh, vmap)
    if wdata:
        tmpl = copy.deepcopy(g_template_weights)
        tmpl.subst("MODEL_NAME", name)
        tmpl.subst("WEIGHT_DATA", wdata)
        ret += "\n\n" + tmpl.format()
    return ret

#def poseToString():
#  for ii in range(0, len(obj.pose_library.pose_markers)):
#    frame = Frame(obj.pose_library.pose_markers[ii], ii, basename)

def armatureBoneDataToString(arm, mat, armature_scale, export_scale):
    """Converts armature bone data to string."""
    ret = []
    for ii in arm.bones:
        hd = mat @ copy.deepcopy(ii.head_local)
        #hd = copy.deepcopy(ii.head_local)
        hd[0] = toExportS16(hd[0] * armature_scale[0] * export_scale)
        hd[1] = toExportS16(hd[1] * armature_scale[1] * export_scale)
        hd[2] = toExportS16(hd[2] * armature_scale[2] * export_scale)
        ret += ["%s%i, %i, %i," % (g_indent, hd[0], hd[1], hd[2])]
    return "\n".join(ret)

def findBoneIndex(arm, bone):
    """Find index of given bone in an armature."""
    for ii in range(len(arm.bones)):
        if arm.bones[ii] == bone:
            return ii
    return -1

def armatureBoneHierarchyToString(arm):
    """Converts armature relation data to string."""
    ret = []
    for ii in arm.bones:
        child_list = "%s%i," % (g_indent, len(ii.children))
        for jj in ii.children:
            idx = findBoneIndex(arm, jj)
            if 0 > idx:
                raise RuntimeError("could not locate bone '%s' index" % (jj.name))
            child_list += " %i," % (idx)
        ret += [child_list]
    return "\n".join(ret)

def armatureToString(arm, name, mat, armature_scale, export_scale):
    """Returns C++ code string from an armature."""
    tmpl = copy.deepcopy(g_template_armature)
    tmpl.subst("MODEL_NAME", name)
    tmpl.subst("BONE_DATA", armatureBoneDataToString(arm, mat, armature_scale, export_scale))
    tmpl.subst("ARMATURE_DATA", armatureBoneHierarchyToString(arm))
    return tmpl.format()

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
        pose_list = ret[kk]
        ret[kk] = sorted(pose_list)
    return ret

def animToString(context, anim, amap, arm, name, key_name, mat, armature_scale, export_scale):
    """Exports singular animation to string."""
    ret = []
    for ii in anim:
        ret += ["%s%i," % (g_indent, toExport8F8(ii.getTime()))]
        bpy.ops.poselib.apply_pose(pose_index=ii.getIndex())
        # Iterate using armature order.
        for jj in amap:
            bone_orig = arm.data.bones[jj]
            bone_curr = arm.pose.bones[jj]
            hd = mat @ copy.deepcopy(bone_curr.head)
            #hd = copy.deepcopy(bone_curr.head)
            px = toExportS16(hd[0] * armature_scale[0] * export_scale)
            py = toExportS16(hd[1] * armature_scale[1] * export_scale)
            pz = toExportS16(hd[2] * armature_scale[2] * export_scale)
            # Heaven knows why we have to rearrange the order
            qq = toExportQuaternion(bone_orig, bone_curr)
            qw = toExport4F12(qq[0])
            qx = toExport4F12(qq[1])
            qy = toExport4F12(qq[2])
            qz = toExport4F12(qq[3])
            ret += ["%s%i, %i, %i, %i, %i, %i, %i," % (g_indent, px, py, pz, qw, qx, qy, qz)]
    tmpl = copy.deepcopy(g_template_anim)
    tmpl.subst("ANIM_DATA", "\n".join(ret))
    tmpl.subst("MODEL_NAME", name)
    tmpl.subst("ANIM_NAME", key_name)
    return tmpl.format()

def exportAllMeshesToHeader(filename, context):
    export_strings = []
    msh = None
    for ii in context.selectable_objects:
        if isExportName(ii.name) and ("MESH" == ii.type):
            msh = ii
            break
    if not msh:
        raise RuntimeError("could not find mesh to export")
    vgl_log("selected mesh for export: '%s'" % (msh.name))
    # Everything needs to be exported in the same scale, discard two bits of accuracy and fit in int16_t.
    export_scale = 32767.0 / (meshFindMaxVertexValue(msh.data, msh.scale) * 4.0)
    # Find armature.
    arm = None
    for ii in context.selectable_objects:
        if isExportName(ii.name) and ("ARMATURE" == ii.type):
            arm = ii
    # Not having scale 1 is suspicious
    if (msh.scale != arm.scale) or (msh.scale != Vector((1.0, 1.0, 1.0))) or (arm.scale != Vector((1.0, 1.0, 1.0))):
        vgl_log("WARNING: suspicious mesh/armature scales: %s ; %s" % (str(msh.scale), str(arm.scale)))
    if arm:
        vgl_log("selected armature for export: '%s'" % (arm.name))
        exp_name = toExportName(arm.name)
        amap = getArmatureOrdering(arm.data)
        vgl_log("Bone mapping: %s" % (str(amap)))
        export_strings += [armatureToString(arm.data, exp_name, arm.matrix_basis, arm.scale, export_scale)]
        # Animations in armatures.
        if hasattr(arm, "pose_library") and arm.pose_library:
            anims = collectPoses(arm.pose_library)
            for (kk, vv) in anims.items():
                export_strings += [animToString(context, vv, amap, arm, exp_name, kk, arm.matrix_basis, arm.scale, export_scale)]
    # Mesh.
    vmap = None
    if arm:
        vmap = getVertexGroupOrdering(msh, amap)
        vgl_log("vertex mapping: %s" % (str(vmap)))
    export_name = toExportName(msh.name)
    export_strings += [meshToString(msh.data, vmap, export_name, msh.scale, export_scale)]
    with open(filename, "w") as fd:
        tmpl = copy.deepcopy(g_template_header)
        tmpl.subst("MODEL_DATA", "\n\n".join(export_strings))
        tmpl.subst("HEADER_NAME", "MODEL_" + export_name.upper() + "_HEADER")
        fd.write(tmpl.format())

########################################
# Blender integration ##################
########################################

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
        )

class ExportVGL(Operator, ExportHelper):
    bl_idname = "export_mesh.cpp"
    bl_label = "Export Faemiyah C++"
    filename_ext = ".hpp"

    filter_glob = StringProperty(default="*.hpp", options={'HIDDEN'})
    test_prop = FloatProperty(
            name="Test property",
            description="Just a property for testing shit",
            min=-1.0, max=1.0,
            default=0.0)

    def execute(self, context):
        filu = self.filepath
        exportAllMeshesToHeader(filu, context)
        return {'FINISHED'}

def menu_func_export(self, context):
    default_path = os.path.splitext(bpy.data.filepath)[0] + ".hpp"
    self.layout.operator(ExportVGL.bl_idname, text="VGL C++ header (.hpp)")

classes = [
    ExportVGL,
]

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
    for cls in classes:
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()
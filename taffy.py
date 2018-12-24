#! /usr/bin/python3
import sys
import re

STRING_RW = 'float rw'
FLOAT_RW = 'float rw'
FLOAT_R = 'float r'
INT_RW = 'int rw'
INT_R = 'int r'
BOOL_RW = 'bool rw'
BOOL_R = 'bool r'
NUM_Q_VARIABLES = 64

preset_vars = {}
matrix_vars = {}
shape_vars = {}
# TODO point_vars
wave_vars = {}

def init_variable_definitions():
    for s in ["frating", "fwavescale", "gamma", "fGammaAdj", "echo_zoom", "fVideoEchoZoom", "echo_alpha", "fvideoechoalpha",
              "wave_a", "fwavealpha",
              "fwavesmoothing", "fmodwavealphastart", "fmodwavealphaend", "fWarpAnimSpeed", "fWarpScale", "fshader",
              "decay", "fdecay",
              "b1n", "b2n", "b3n", "b1x", "b2x", "b3x", "b1ed", "wave_r", "wave_g", "wave_b", "wave_x", "wave_y",
              "wave_mystery", "fWaveParam",
              "ob_size", "ob_r", "ob_g", "ob_b", "ob_a",
              "ib_size", "ib_r", "ib_g", "ib_b", "ib_a",
              "mv_r", "mv_g", "mv_b", "mv_x", "nmotionvectorsx", "mv_y", "nmotionvectorsy", "mv_l", "mv_dx", "mv_dy",
              "mv_a"]:
        preset_vars[s.lower()] = FLOAT_RW
    for s in ["echo_orient", "nVideoEchoOrientation", "wave_mode", "nwavemode"]:
        preset_vars[s.lower()] = INT_RW
    for s in ["wave_additive", "bAdditiveWaves", "bmodwavealphabyvolume", "wave_brighten", "bMaximizeWaveColor", "wrap",
              "btexwrap",
              "darken_center", "bdarkencenter", "bredbluestereo", "brighten", "bbrighten", "darken", "bdarken", "solarize",
              "bsolarize",
              "invert", "binvert", "bmotionvectorson", "wave_dots", "bwavedots", "wave_thick", "bwavethick"]:
        preset_vars[s.lower()] = BOOL_RW

    # read-only
    for s in ["time", "bass", "mid", "treb", "bass_att", "mid_att", "treb_att", "progress"]:
        preset_vars[s] = FLOAT_R
    preset_vars['frame'] = INT_R
    preset_vars['fps'] = INT_R
    preset_vars['meshx'] = INT_R
    preset_vars['meshy'] = INT_R

    # matrix variables
    for s in ["warp", "zoom", "rot", "zoomexp", "fzoomexponent", "cx", "cy", "dx", "dy", "sx", "sy"]:
        preset_vars[s] = FLOAT_RW
        matrix_vars[s] = FLOAT_RW
    for s in ["x", "y", "ang", "rad"]:
        matrix_vars[s] = FLOAT_R

    for i in range(0, NUM_Q_VARIABLES):
        preset_vars['q' + str(i)] = FLOAT_RW
        matrix_vars['q' + str(i)] = FLOAT_RW

    # shape
    for s in ["r", "g", "b", "a", "border_r", "border_g", "border_b", "border_a", "r2", "g2", "b2", "a2", "x", "y",
              "thickoutline", "enabled", "sides", "additive", "textured", "rad", "ang", "tex_zoom", "tex_ang",
              ]:
        shape_vars[s] = FLOAT_RW
    for i in range(0, 8):
        shape_vars['t' + str(i)] = FLOAT_RW
    for i in range(0, NUM_Q_VARIABLES):
        shape_vars['q' + str(i)] = FLOAT_RW
    shape_vars['imageurl'] = STRING_RW
    # TODO shape_points


    # wave
    for s in ["r", "g", "b", "a", "x", "y", "smoothing", "scaling"]:
        wave_vars[s] = FLOAT_RW
    for s in ["enabled", "bspectrum", "bdrawthick", "busedots", "badditive"]:
        wave_vars[s] = BOOL_RW
    for s in ["sep", "samples"]:
        wave_vars[s] = INT_RW
    for i in range(0, 8):
        wave_vars['t' + str(i)] = FLOAT_RW
    for i in range(0, NUM_Q_VARIABLES):
        wave_vars['q' + str(i)] = FLOAT_RW


    # TODO wave_points
    # int [sample int_r
    # float value1 float_r
    # float value2 float_r


class Shape:
    def __init__(self, name):
        self.name = str(name)
        self.frame_eqns = []
        self.point_eqns = []
        self.variables = []
        self.comments = []


class Preset:
    def __init__(self):
        self.name = ''
        self.variables = []
        self.frame_init_eqns = []
        self.frame_eqns = []
        self.pixel_eqns = []
        self.waves = []
        self.shapes = []
        self.comp_lang = 'hlsl'
        self.comp = []
        self.warp_lang = 'hlsl'
        self.warp = []
        self.comments = []

    def get_shape(self, i):
        i = int(i)
        while len(self.shapes) <= int(i):
            self.shapes.append(Shape(len(self.shapes)))
        return self.shapes[i]

    def get_wave(self, i):
        i = int(i)
        while len(self.waves) <= int(i):
            self.waves.append(Shape(len(self.waves)))
        return self.waves[i]


# 'parse' right hand side of expression, allow substition of identifiers
def scan_expr(expr, ident_callback):
    last = 0
    ret = []
    for m in re.finditer(r'[\w$]+(\s*\()?', expr):
        ret.append(expr[last:m.start(0)])
        last = m.end(0)
        if (m.group(0).endswith('(')):
            ret.append(m.group(0))
            continue
        ret.append(ident_callback(m.group(0)))
    ret.append(expr[last:])
    return ''.join(ret)


##
## MILKDROP
##


def parse_milk(lines):
    preset = Preset()
    for line in lines:
        m = re.match(r'shape_(\d+)_per_(point|frame)(\d+)\s*=', line)
        if m:
            eqn = line[(len(m.group(0))):].strip()
            shape = preset.get_shape(m.group(1))
            if 'frame' == m.group(2):
                shape.frame_eqns.append(eqn)
            if 'point' == m.group(2):
                shape.point_eqns.append(eqn)
            continue
        m = re.match(r'shapecode_(\d+)_(\w+)\s*=', line)
        if m:
            rhs = line[(len(m.group(0))):].strip()
            shape = preset.get_shape(m.group(1))
            shape.variables.append(m.group(2) + '=' + rhs)
            continue
        m = re.match(r'wave_(\d+)_per_(point|frame)(\d+)\s*=', line)
        if m:
            eqn = line[(len(m.group(0))):].strip()
            wave = preset.get_wave(m.group(1))
            if 'frame' == m.group(2):
                wave.frame_eqns.append(eqn)
            if 'point' == m.group(2):
                wave.point_eqns.append(eqn)
            continue
        m = re.match(r'wavecode_(\d+)_(\w+)\s*=', line)
        if m:
            rhs = line[(len(m.group(0))):].strip()
            wave = preset.get_wave(m.group(1))
            wave.variables.append(m.group(2) + '=' + rhs)
            continue
        m = re.match(r'per_(frame_init|frame|pixel)_(\d+)\s*=', line)
        if m:
            eqn = line[(len(m.group(0))):].strip()
            if 'frame_init' == m.group(1):
                preset.frame_init_eqns.append(eqn)
            elif 'frame' == m.group(1):
                preset.frame_eqns.append(eqn)
            elif 'pixel' == m.group(1):
                preset.pixel_eqns.append(eqn)
            continue
        m = re.match(r'(comp|warp)_(\d+)\s*=', line)
        if m:
            code = line[(len(m.group(0))):]
            if code.startswith('`'):
                code = code[1:]
            if 'comp' == m.group(1):
                preset.comp.append(code)
            else:
                preset.warp.append(code)
            continue

        m = re.match(r'\[(\w+)\]', line)
        if m:
            if m.group(1) != 'preset00':
                preset.name = m.group(1)
            continue

        m = re.match(r'(\w+)\s*=', line)
        if m:
            if 'comp_lang' == m.group(1).strip():
                preset.comp_lang = line.substr(len(m.group(1))).strip()
            if 'warp_lang' == m.group(1).strip():
                preset.warp_lang = line.substr(len(m.group(1))).strip()
            preset.variables.append(line.strip())
        continue

    return preset


def dump_shape_milk(output, type, index, shape, scope=None, scope_vars=None):
    prefix = type + "code_" + str(index) + "_"
    for v in shape.variables:
        output.write(prefix + v + "\n")
    prefix = type + "_" + str(index) + "_per_frame"
    for i in range(0, len(shape.frame_eqns)):
        eqn = shape.frame_eqns[i]
        output.write(prefix + str(i + 1) + '=' + eqn + "\n")
    prefix = type + "_" + str(index) + "_per_point"
    for i in range(0, len(shape.point_eqns)):
        eqn = shape.point_eqns[i]
        output.write(prefix + str(i + 1) + '=' + eqn + "\n")


def dump_milk(output, preset, var_prefix=None):
    output.write("[" + (preset.name||"preset00") + "]\n")
    for variable in preset.variables:
        output.write(variable + "\n")
    for i in range(0, len(preset.frame_init_eqns)):
        eqn = preset.frame_init_eqns[i]
        output.write("per_frame_init_" + str(i + 1) + "=" + eqn + "\n")
    for i in range(0, len(preset.frame_eqns)):
        eqn = preset.frame_eqns[i]
        output.write("per_frame_" + str(i + 1) + "=" + eqn + "\n")
    for i in range(0, len(preset.pixel_eqns)):
        eqn = preset.pixel_eqns[i]
        output.write("per_pixel_" + str(i + 1) + "=" + eqn + "\n")

    for i in range(0, len(preset.shapes)):
        dump_shape_milk(output, "shape", i, preset.shapes[i])

    for i in range(0, len(preset.waves)):
        dump_shape_milk(output, "wave", i, preset.waves[i])

    for i in range(0, len(preset.comp)):
        output.write("comp_" + str(i + 1) + '=`' + preset.comp[i])
    for i in range(0, len(preset.warp)):
        output.write("warp_" + str(i + 1) + '=`' + preset.warp[i])

    return


##
## TAFFY
##


BEGIN = '{'
END= '}'
OBJECT='var'
FUNCTION='%s = {'
SHADER_BEGIN='%s_%s = """'
SHADER_END='"""'
LINE_COMMENT = '//'


USE_SCOPE = True
VERBOSE = True


def format_eqn(assignments, eqn, scope=None, scope_vars=None, outer_scope=None, outer_vars=None, var_prefix=None):
    # TODO parse RHS of equations
    eqn = eqn.strip()
    if not eqn:
        return ""
    if eqn.startswith("//"):
        return LINE_COMMENT + eqn[2:]
    if -1 == eqn.find('='):
        return eqn
    if not scope_vars:
        return eqn

    var = eqn[0:eqn.find('=')].strip()
    expr = eqn[eqn.find('=') + 1:]

    def rename_user_var(ident):
        return (var_prefix + ident) if ident in assignments and assignments[ident] == 'var' else ident

    if var_prefix:
        expr = scan_expr(expr, rename_user_var)

    if var.lower() in scope_vars:
        if assignments is not None:
            assignments[var] = scope
        if VERBOSE and (scope_vars[var.lower()] == FLOAT_R or scope_vars[var.lower()] == INT_R or scope_vars[
            var.lower()] == BOOL_R):
            return var + " = " + expr + "    # assigning to READ-ONLY variable?"
        else:
            return var + " = " + expr
    elif assignments is not None and not var in assignments:
        assignments[var] = 'var'

    if var_prefix:
        var = rename_user_var(var)
    return var + " = " + expr


def dump_eqns(output, eqns, scope=None, scope_vars=None, outer_scope=None, outer_vars=None, var_prefix=None):
    if VERBOSE:
        assignments = {}
        for eqn in eqns:
            format_eqn(assignments, eqn, scope, scope_vars, outer_scope, outer_vars)
        user_variables = []
        for k, v in assignments.items():
            if v == 'var':
                user_variables.append(k)
        if len(user_variables):
            output.write('    ' + LINE_COMMENT + ' user variables: ' + ','.join(user_variables) + '\n')
    assignments = {}
    for eqn in eqns:
        line = format_eqn(assignments, eqn, scope, scope_vars, outer_scope, outer_vars, var_prefix=var_prefix)
        output.write('    ' + line + "\n")


def format_var(var, scope=None, scope_vars=None):
    return var


def dump_shape(output, type, shape, scope=None, scope_vars=None, var_prefix=None):
    output.write("\n" + type + " '" + shape.name + "' " + BEGIN + "\n")

    for v in shape.variables:
        output.write("  " + format_var(v, scope, scope_vars) + "\n")

    if (len(shape.frame_eqns)):
        output.write("\n  " + (FUNCTION % "per_frame") + "\n")
        assignments = {}
        for eqn in shape.frame_eqns:
            output.write("    " + format_eqn(assignments, eqn, "shape", scope_vars) + "\n")
        output.write("  " + END + "\n")
    if (len(shape.point_eqns)):
        output.write("\n  " + (FUNCTION % "per_point") + "\n")
        dump_eqns(output, shape.point_eqns, scope, scope_vars, var_prefix=var_prefix)
        output.write("  " + END + "\n")
    output.write(END + "\n")


def dump_taffy(output, preset, var_prefix=None):

    if preset.name:
        output.write("preset '" + preset.name + "' " + BEGIN + "\n")
    else:
        output.write("preset " + BEGIN + "\n")

    for variable in preset.variables:
        if variable.startswith("comp_lang") or variable.startswith("warp_lang"):
            continue
        output.write("  " + format_var(variable, "preset", preset_vars) + "\n")

    if len(preset.frame_init_eqns):
        output.write("\n  " + (FUNCTION % "per_frame_init") + "\n")
        assignments = {}
        for eqn in preset.frame_init_eqns:
            output.write("    " + format_eqn(assignments, eqn, "frame") + "\n")
        output.write("  " + END + "\n")

    if len(preset.frame_eqns):
        output.write("\n  " + (FUNCTION % "per_frame") + "\n")
        dump_eqns(output, preset.frame_eqns, "frame", preset_vars, var_prefix=var_prefix)
        output.write("  " + END + "\n")

    if len(preset.pixel_eqns):
        output.write("\n  " + (FUNCTION % "per_pixel") + "\n")
        dump_eqns(output, preset.pixel_eqns, "pixel", matrix_vars, "frame", preset_vars, var_prefix=var_prefix)
        output.write("  " + END + "\n")

    if preset.comp:
        output.write('\n  ' + (SHADER_BEGIN % ('comp', preset.comp_lang)) + '\n')
        for s in preset.comp:
            output.write("    " + s)
        output.write(SHADER_END + '\n')

    if preset.warp:
        output.write('\n  ' + (SHADER_BEGIN % ('warp', preset.warp_lang)) + '\n')
        for s in preset.warp:
            output.write("    " + s)
        output.write(SHADER_END + '\n')

    output.write(END + "\n\n")

    for shape in preset.shapes:
        dump_shape(output, "shape", shape, "shape", shape_vars)

    for wave in preset.waves:
        dump_shape(output, "wave", wave, "wave", wave_vars)


init_variable_definitions()

if __name__ == "__main__":
    input_file = None
    input = sys.stdin
    output_file = None
    output = sys.stdout
    parse = parse_milk
    dump = dump_taffy
    for arg in sys.argv[1:]:
        if arg == '--taffy':
            pass
        elif arg == '--milk':
            # parse = parse_taffy
            dump = dump_milk
        elif not input_file:
            input_file = arg
        elif not output_file:
            output_file = arg
        else:
            raise Exception("too many arguments")
    if input_file and input_file != "-":
        input = open(input_file, 'r')
        if input_file.endswith('.salt'):
            raise Exception("parse_saltwatertaffy is NYI")
    if output_file and output_file != "-":
        output = open(output_file, 'w')
        if output_file.endswith('.milk'):
            dump = dump_milk

    try:
        preset = parse(input)
        if not preset.name or preset.name == 'preset00':
            if input_file != '-':
                preset.name = input_file
        dump(output, preset, var_prefix='')
    except UnicodeDecodeError as ex:
        sys.stdout.write("error parsing file: " + input_file + "\n")

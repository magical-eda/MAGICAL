from .glovar import tsmc40_glovar as glovar
import math

min_w = glovar.min_w
GRID = glovar.GRID

def legal(length,ceil=True):
    # type 0 is ceil, 1 is foor
    if abs(round(length/(min_w['SP']+min_w['M1'])) * (min_w['SP']+min_w['M1']) - length) < GRID - 0.001:
        return length
    if ceil:
        return int(math.ceil(length/(min_w['SP']+min_w['M1']))) * (min_w['SP']+min_w['M1'])
    return int(math.floor(length/(min_w['SP']+min_w['M1']))) * (min_w['SP']+min_w['M1'])

def check_legal_coord(coord, origin=[0,0]):
    if legal(coord[0]-origin[0]) == coord[0]-origin[0] and legal(coord[1]-origin[1]) == coord[1]-origin[1]:
        return True
    print(coord, origin)
    return False


class Pin:
    def __init__(self, name):
        self.name = name
        self.shape = []

    def add_shape(self, layer, cord_list):
        self.shape.append([layer, list(cord_list[0]), list(cord_list[1])])

    def adjust(self, offset):
        for shape in self.shape:
            shape[1][0] = shape[1][0] - offset[0]
            shape[1][1] = shape[1][1] - offset[1]
            shape[2][0] = shape[2][0] - offset[0]
            shape[2][1] = shape[2][1] - offset[1]

    def check(self, origin=[-0.5*min_w['M1'], -0.5*min_w['M1']]):
        for shape in self.shape:
            if not check_legal_coord(shape[1], origin):
                print(shape[1], origin)
                return False
            if not check_legal_coord([shape[2][0]+min_w['SP'],shape[2][1]+min_w['SP']], origin):
                print(shape[2], origin)
                return False
        return True

    def flip_vert(self, axis):
        for shape in self.shape:
            temp = axis - shape[1][0] 
            shape[1][0] = axis - shape[2][0]
            shape[2][0] = temp

    def normalize_shape(self, unit=1e-9):
        shape = self.shape[0]
        return_shape = list()
        return_shape.append(int(shape[0][1]))
        return_shape.append(int(round(shape[1][0]*1.0e-6/unit)))
        return_shape.append(int(round(shape[1][1]*1.0e-6/unit)))
        return_shape.append(int(round(shape[2][0]*1.0e-6/unit)))
        return_shape.append(int(round(shape[2][1]*1.0e-6/unit)))
        return return_shape

    def normalize(self, unit=1e-9):
        return_shape = self.normalize_shape(unit)
        shape_layer = self.shape[0][0]
        temp = '%s %d %d %d %d\n' % (shape_layer, return_shape[1], return_shape[2], return_shape[3], return_shape[4])
        return temp

    def __str__(self):
        _string = '%s    %d\n' % (self.name, len(self.shape))
        for shape in self.shape:
            temp = '\t%s    ((%.3f %.3f) (%.3f %.3f))\n' % (shape[0], shape[1][0], shape[1][1], shape[2][0], shape[2][1])
            _string = _string + temp
        return _string
        #if self.name == 'BULK':
        #    _string = '%s    %d\n' % (self.name, len(self.shape))
        #    for shape in self.shape:
        #        temp = ' %s    ((%.3f %.3f) (%.3f %.3f)) ' % (shape[0], shape[1][0], shape[1][1], shape[2][0], shape[2][1])
        #        _string = _string + temp
        #    _string += '\n'
        #else:
        #    _string = '%s    1\n' % self.name
        #    shape = self.shape[0]
        #    temp = ' %s    ((%.3f %.3f) (%.3f %.3f))\n' % (shape[0], shape[1][0], shape[1][1], shape[2][0], shape[2][1])
        #    _string = _string + temp
        #return _string
    

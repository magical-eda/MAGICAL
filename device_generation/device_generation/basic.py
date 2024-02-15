from .glovar import tsmc40_glovar as glovar
import math
import gdspy
from .Pin import Pin

# Global Variable Design Rules from glovar.py
class basic:
    min_w = glovar.min_w
    layer = glovar.layer
    datatype = glovar.datatype
    sp = glovar.sp
    en = glovar.en
    ex = glovar.ex
    NWELL_GR = glovar.NWELL_GR
    NW_OD = glovar.NW_OD
    NP_OD = glovar.NP_OD
    OD_W = glovar.OD_W
    GRID = glovar.GRID

# Rules for VIAs
    W_VIA = 0.05 
    SP_VIA = 0.1
    EN_VIA = 0.05 

    @staticmethod
    def block(inst):
        bound = inst.cell.get_bounding_box()
        bound[0][0] = math.floor(bound[0][0]/0.01)*0.01
        bound[0][1] = math.floor(bound[0][1]/0.01)*0.01
        bound[1][0] = math.ceil((bound[1][0]+0.005)/0.01)*0.01
        bound[1][1] = math.ceil((bound[1][1]+0.005)/0.01)*0.01
        x = int((bound[1][0] - bound[0][0]) * 100)
        y = int((bound[1][1] - bound[0][1]) * 100)
        return [x, y]

    @staticmethod
    def BB(inst, flipCell=False, unit=1e-9):
        bound_box = inst.cell.get_bounding_box()
        bound = [0,0]
        bound[0] = list(basic.legal_coord(bound_box[0],[0,0],1))
        bound[1] = list(basic.legal_coord(bound_box[1],[0,0],3))
        # why 1.5: w/l must be odd multiple of gridStep due to placement assumption
        if int((bound[1][0] - bound[0][0])*1000) % int((2*(basic.min_w['M1']+basic.min_w['SP']))*1000) < 1e-9:
            x_kappa = 0.0
        else:
            x_kappa = 1.0
        if int((bound[1][1] - bound[0][1])*1000) % int((2*(basic.min_w['M1']+basic.min_w['SP']))*1000) < 1e-9:
            y_kappa = 0.0
        else:
            y_kappa = 0.0
        if flipCell:
            bound[0][0] = bound[0][0] - x_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        else:
            bound[1][0] = bound[1][0] + x_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        #bound[0][1] = bound[0][1] #- y_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        #bound[1][0] = bound[1][0] #+ x_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        #bound[1][1] = bound[1][1] #+ y_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        return_bound = list()
        return_bound.append(int(round(bound[0][0]*1.0e-6/unit)))
        return_bound.append(int(round(bound[0][1]*1.0e-6/unit)))
        return_bound.append(int(round(bound[1][0]*1.0e-6/unit)))
        return_bound.append(int(round(bound[1][1]*1.0e-6/unit)))
        return return_bound

    @staticmethod
    def BB_list(bound_list, unit=1e-9):
        bound = [0,0]
        halfGrid = 0.5 * basic.min_w['M1']
        bound[0] = list(basic.legal_coord([int(bound_list[0])/1000.0,int(bound_list[1])/1000.0],[0,0],1))
        bound[1] = list(basic.legal_coord([int(bound_list[2])/1000.0,int(bound_list[3])/1000.0],[0,0],3))
        # why 1.5: w/l must be odd multiple of gridStep due to placement assumption
        if int((bound[1][0] - bound[0][0])*1000) % int((2*(basic.min_w['M1']+basic.min_w['SP']))*1000) < 1e-9:
            x_kappa = 0.0
        else:
            x_kappa = 1.0
        if int((bound[1][1] - bound[0][1])*1000) % int((2*(basic.min_w['M1']+basic.min_w['SP']))*1000) < 1e-9:
            y_kappa = 0.0
        else:
            y_kappa = 0.0
        bound[0][0] = bound[0][0] - x_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        bound[0][1] = bound[0][1] #- y_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        bound[1][0] = bound[1][0] #+ x_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        bound[1][1] = bound[1][1] #+ y_kappa * (basic.min_w['M1'] + basic.min_w['SP'])
        return_bound = list()
        return_bound.append(int(round(bound[0][0]*1.0e-6/unit)))
        return_bound.append(int(round(bound[0][1]*1.0e-6/unit)))
        return_bound.append(int(round(bound[1][0]*1.0e-6/unit)))
        return_bound.append(int(round(bound[1][1]*1.0e-6/unit)))
        return return_bound

    @staticmethod
    def legalize_pin_from_dumb(lo, hi, plane):
        if plane == 3:
            newshape = legal_coord([lo/1000.0, hi/1000.0],[-0.5*basic.min_w['M1'],-0.5*basic.min_w['M1']],plane)
            return (int(newshape[0]*1000), int(newshape[1]*1000))
        else:
            assert plane == 1, "Legal coord must be ll or ur"
            newshape = legal_coord([lo/1000.0-basic.min_w['M1'], hi/1000.0-basic.min_w['M1']],[-0.5*basic.min_w['M1'],-0.5*basic.min_w['M1']],plane)
            return (int((newshape[0]+basic.min_w['M1'])*1000), int((newshape[1]+basic.min_w['M1'])*1000))

    @staticmethod
    def legal(length,ceil=True):
        # type 0 is ceil, 1 is foor
        if abs(round(length/(basic.min_w['SP']+basic.min_w['M1'])) * (basic.min_w['SP']+basic.min_w['M1']) - length) < basic.GRID - 0.001:
            return length
        if ceil:
            return int(math.ceil(length/(basic.min_w['SP']+basic.min_w['M1']))) * (basic.min_w['SP']+basic.min_w['M1'])
        return int(math.floor(length/(basic.min_w['SP']+basic.min_w['M1']))) * (basic.min_w['SP']+basic.min_w['M1'])

    @staticmethod
    def legal_len(length):
        if length <= basic.min_w['M1']:
            return basic.min_w['M1']
        return basic.legal(length-basic.min_w['M1'])+basic.min_w['M1']

    @staticmethod
    def legal_coord(coord, origin, plane=1):
        # legalize ll in rectangle
        # plane: 1,2,3,4 in planer relative to lattice point
        if plane in [1,4]:
            x_ceil = False
        else:
            x_ceil = True
        if plane in [1,2]:
            y_ceil = False
        else:
            y_ceil = True
        x = basic.legal(coord[0]-origin[0],x_ceil) + origin[0]
        y = basic.legal(coord[1]-origin[1],y_ceil) + origin[1]
        return x,y
                              
    @staticmethod
    def offset(inst):
        return inst.cell.get_bounding_box()[0]

    @staticmethod
    def contact(lay=0):
        contact_cell = gdspy.Cell('CONTACT', True)
        if lay == 0:
            contact_shape = gdspy.Rectangle((0, 0), (basic.min_w['CO'], basic.min_w['CO']), basic.layer['CO'])
        else:
            contact_shape = gdspy.Rectangle((0, 0), (basic.W_VIA, basic.W_VIA), basic.layer['VIA'+str(lay)], basic.datatype['VIA'+str(lay)])
        contact_cell.add(contact_shape)
        return contact_cell

    @staticmethod
    def power_strip(w, h, offset=[0,0], lay=[5,6]):
        power_cell = gdspy.Cell("POWER", True)
        contact_num_w = int((w-2*basic.EN_VIA+basic.SP_VIA)/(basic.W_VIA+basic.SP_VIA))
        contact_num_h = int((h-2*basic.EN_VIA+basic.SP_VIA)/(basic.W_VIA+basic.SP_VIA))
        contact_space_w = (w-basic.EN_VIA-contact_num_w*basic.W_VIA)/(contact_num_w-1)+basic.W_VIA
        contact_space_h = (h-basic.EN_VIA-contact_num_h*basic.W_VIA)/(contact_num_h-1)+basic.W_VIA
        if contact_space_w < basic.SP_VIA + basic.W_VIA:
            contact_num_w = contact_num_w - 1 
            contact_space_w = (h-2*basic.EN_VIA-contact_num_w*basic.W_VIA)/(contact_num_w-1)+basic.W_VIA
        if contact_space_h < basic.SP_VIA + basic.W_VIA:
            contact_num_h = contact_num_h - 1 
            contact_space_h = (h-2*basic.EN_VIA-contact_num_h*basic.W_VIA)/(contact_num_h-1)+basic.W_VIA
        contact_space_h = int(contact_space_h/basic.GRID)*basic.GRID  
        contact_space_w = int(contact_space_w/basic.GRID)*basic.GRID  
        x_offset = (w-basic.W_VIA-contact_space_w*(contact_num_w-1))/2 + offset[0]
        y_offset = (h-basic.W_VIA-contact_space_h*(contact_num_h-1))/2 + offset[1]
        x_offset = int(x_offset/basic.GRID)*basic.GRID
        y_offset = int(y_offset/basic.GRID)*basic.GRID
        for i in lay:
            met_layer = 'M' + str(i)
            m1_shape = gdspy.Rectangle((offset[0],offset[1]),(offset[0]+w,offset[1]+h),basic.layer[met_layer], basic.datatype[met_layer])
            power_cell.add(m1_shape)
            if i != lay[-1]:
                contact_cell = basic.contact(i)
                contact_array = gdspy.CellArray(contact_cell, contact_num_w, contact_num_h, [contact_space_w, contact_space_h], [x_offset, y_offset])
                power_cell.add(contact_array)
        power_cell.flatten()
        return power_cell

    @staticmethod
    def power_pin_init(ll, ur, startlay=2, stoplay=6):
        w = ur[0] - ll[0]
        h = ur[1] - ll[1]
        offset = ll
        init_cell = gdspy.Cell("POWER", True)
        contact_num_w = int((w-2*basic.EN_VIA+basic.SP_VIA)/(basic.W_VIA+basic.SP_VIA))
        contact_num_h = int((h-2*basic.EN_VIA+basic.SP_VIA)/(basic.W_VIA+basic.SP_VIA))
        if contact_num_w <= 1:
            contact_num_w = 1
            contact_space_w = basic.SP_VIA + basic.W_VIA
        else:
            contact_space_w = (w-basic.EN_VIA-contact_num_w*basic.W_VIA)/(contact_num_w-1)+basic.W_VIA
        if contact_num_h <= 1:
            contact_num_h = 1
            contact_space_h = basic.SP_VIA + basic.W_VIA
        else:
            contact_space_h = (h-basic.EN_VIA-contact_num_h*basic.W_VIA)/(contact_num_h-1)+basic.W_VIA
        if contact_space_w < basic.SP_VIA + basic.W_VIA:
            contact_num_w = contact_num_w - 1 
            if contact_num_w <= 1:
                contact_num_w = 1
                contact_space_w = basic.SP_VIA + basic.W_VIA
            else:
                contact_space_w = (h-2*basic.EN_VIA-contact_num_w*basic.W_VIA)/(contact_num_w-1)+basic.W_VIA
        if contact_space_h < basic.SP_VIA + basic.W_VIA:
            contact_num_h = contact_num_h - 1 
            if contact_num_h <= 1:
                contact_num_h = 1
                contact_space_h = basic.SP_VIA + basic.W_VIA
            else:
                contact_space_h = (h-2*basic.EN_VIA-contact_num_h*basic.W_VIA)/(contact_num_h-1)+basic.W_VIA
        contact_space_h = int(contact_space_h/basic.GRID)*basic.GRID  
        contact_space_w = int(contact_space_w/basic.GRID)*basic.GRID  
        x_offset = (w-basic.W_VIA-contact_space_w*(contact_num_w-1))/2 + offset[0]
        y_offset = (h-basic.W_VIA-contact_space_h*(contact_num_h-1))/2 + offset[1]
        x_offset = int(x_offset/basic.GRID)*basic.GRID
        y_offset = int(y_offset/basic.GRID)*basic.GRID
        for i in range(startlay, stoplay+1):
            met_layer = 'M' + str(i)
            m1_shape = gdspy.Rectangle((offset[0],offset[1]),(offset[0]+w,offset[1]+h),basic.layer[met_layer], basic.datatype[met_layer])
            init_cell.add(m1_shape)
            contact_cell = basic.contact(i-1)
            contact_array = gdspy.CellArray(contact_cell, contact_num_w, contact_num_h, [contact_space_w, contact_space_h], [x_offset, y_offset])
            init_cell.add(contact_array)
        init_cell.flatten()
        return init_cell

    @staticmethod
    def metal_vert(w, h, lay=1): #usually w set to min_w['M1']
        met_layer = 'M' + str(lay)
        m1_cell = gdspy.Cell('M1_VERT', True)
        m1_shape = gdspy.Rectangle((0,0), (w,h), basic.layer[met_layer], basic.datatype[met_layer])
        m1_cell.add(m1_shape)
        contact_cell = basic.contact(lay-1)
        if lay == 1:
            contact_num = int((h-2*basic.en['M1']['CO']+basic.sp['CO']['CO'])/(basic.min_w['CO']+basic.sp['CO']['CO']))
        else:
            contact_num = int((h-2*basic.EN_VIA+basic.SP_VIA)/(basic.W_VIA+basic.SP_VIA))
        if contact_num == 1:
            if lay == 1:
                contact_space = basic.sp['CO']['CO']
            else:
                contact_space = basic.SP_VIA
        else:
            if lay == 1:
                contact_space = (h-2*basic.en['M1']['CO']-contact_num* basic.min_w['CO'])/(contact_num-1)+basic.min_w['CO']
                if contact_space < basic.sp['CO']['CO'] + basic.min_w['CO']:
                    contact_num = contact_num - 1 
                    contact_space = (h-2*basic.en['M1']['CO']-contact_num*basic.min_w['CO'])/(contact_num-1)+basic.min_w['CO']
            else:
                contact_space = (h-basic.EN_VIA-contact_num*basic.W_VIA)/(contact_num-1)+basic.W_VIA
                if contact_space < basic.SP_VIA + basic.W_VIA:
                    contact_num = contact_num - 1 
                    contact_space = (h-2*basic.EN_VIA-contact_num*basic.W_VIA)/(contact_num-1)+basic.W_VIA
        contact_space = int(contact_space/basic.GRID)*basic.GRID  # to satisify manufactor grid
        # Offset Center
        if lay == 1:
            x_offset = (w-basic.min_w['CO'])/2
            y_offset = (h - basic.min_w['CO'] - contact_space*(contact_num-1))/2 
        else:
            x_offset = (w-basic.W_VIA)/2
            y_offset = (h - basic.W_VIA - contact_space*(contact_num-1))/2 
        x_offset = int(x_offset/basic.GRID)*basic.GRID
        y_offset = int(y_offset/basic.GRID)*basic.GRID
#    y_offset = en['M1']['CO']
        contact_array = gdspy.CellArray(contact_cell, 1, contact_num, [contact_space, contact_space], [x_offset, y_offset])
        m1_cell.add(contact_array)
        m1_cell.flatten()
        return m1_cell

    @staticmethod
    def metal_hori(w, h, lay=1): #usually h set to basic.min_w['M1']
        met_layer = 'M' + str(lay)
        m1_cell = gdspy.Cell('M1_HORI', True)
        m1_shape = gdspy.Rectangle((0,0), (w,h), basic.layer[met_layer], basic.datatype[met_layer])
        m1_cell.add(m1_shape)
        contact_cell = basic.contact(lay-1)
        if lay == 1:
            contact_num = int((w-2*basic.en['M1']['CO']+basic.sp['CO']['CO'])/(basic.min_w['CO']+basic.sp['CO']['CO']))
        else:
            contact_num = int((w-2*basic.EN_VIA+basic.SP_VIA)/(basic.W_VIA+basic.SP_VIA))
        if contact_num == 1:
            if lay == 1:
                contact_space = basic.sp['CO']['CO']
            else:
                contact_space = basic.SP_VIA
        else:
            if lay == 1:
                contact_space = (w-2*basic.en['M1']['CO']-contact_num*basic.min_w['CO'])/(contact_num-1)+basic.min_w['CO']
                if contact_space < basic.sp['CO']['CO'] + basic.min_w['CO']:
                    contact_num = contact_num - 1 
                    contact_space = (w-2*basic.en['M1']['CO']-contact_num*basic.min_w['CO'])/(contact_num-1)+basic.min_w['CO']
            else:
                contact_space = (w-basic.EN_VIA-contact_num*basic.W_VIA)/(contact_num-1)+basic.W_VIA
                if contact_space < basic.SP_VIA + basic.W_VIA:
                    contact_num = contact_num - 1 
                    contact_space = (w-2*basic.EN_VIA-contact_num*basic.W_VIA)/(contact_num-1)+basic.W_VIA
        contact_space = int(contact_space/basic.GRID)*basic.GRID  # to satisify manufactor grid
        #contact_num = int((w-2*en['M1']['CO']+sp['CO']['CO'])/(basic.min_w['CO']+sp['CO']['CO']))
        #if contact_num ==1:
        #    contact_space = sp['CO']['CO']
        #else:
        #    contact_space = (w-2*en['M1']['CO']-contact_num*basic.min_w['CO'])/(contact_num-1)+basic.min_w['CO']
        #    if contact_space < sp['CO']['CO'] + basic.min_w['CO']:
        #        contact_num = contact_num - 1 
        #        contact_space = (w-2*en['M1']['CO']-contact_num*basic.min_w['CO'])/(contact_num-1)+basic.min_w['CO']
        #contact_space = int(contact_space/basic.GRID)*basic.GRID  # to satisify manufactor grid
        # Offset Center
        if lay == 1:
            x_offset = (w - basic.min_w['CO'] - contact_space*(contact_num-1))/2 
            y_offset = (h-basic.min_w['CO'])/2
        else:
            x_offset = (w - basic.W_VIA - contact_space*(contact_num-1))/2 
            y_offset = (h-basic.W_VIA)/2
        x_offset = int(x_offset/basic.GRID)*basic.GRID
        y_offset = int(y_offset/basic.GRID)*basic.GRID

        contact_array = gdspy.CellArray(contact_cell, contact_num, 1, [contact_space, contact_space], [x_offset, y_offset])
        m1_cell.add(contact_array)
        m1_cell.flatten()
        return m1_cell

# Check all nwell functions if basic.min_w['M1'] > basic.NP_OD

    @staticmethod
    def nwell_vert(h, dope='NP', remove=False, topmet=1): # w set to be minimal by definition
        nwell_vert = gdspy.Cell('NWELL_VERT', True)
        np_w = 2 * basic.NP_OD + basic.OD_W
        np_shape = gdspy.Rectangle((0, 0), (np_w, h), basic.layer[dope])
        nwell_vert.add(np_shape)
        od_shape = gdspy.Rectangle((basic.NP_OD, 0), (basic.NP_OD+basic.OD_W, h), basic.layer['OD'])
        nwell_vert.add(od_shape)
        # Legalization changes
        if not remove:
            for metal in range(topmet):
                m_vert = basic.metal_vert(basic.min_w['M1'], h, metal+1)
                m_vert = gdspy.CellReference(m_vert, (basic.NP_OD+0.5*(basic.OD_W-basic.min_w['M1']), 0))
                nwell_vert.add(m_vert)
        nwell_vert.flatten()
        return nwell_vert

    @staticmethod
    def nwell_hori(w, dope='NP', remove=False, topmet=1): # h set to be minimal by definition
        nwell_hori = gdspy.Cell('NWELL_HORI', True)
        np_h = 2 * basic.NP_OD + basic.OD_W
        np_shape = gdspy.Rectangle((0, 0), (w, np_h), basic.layer[dope])
        nwell_hori.add(np_shape)
        od_shape = gdspy.Rectangle((0, basic.NP_OD), (w, basic.NP_OD+basic.OD_W), basic.layer['OD'])
        nwell_hori.add(od_shape)
        # Legalization changes
        if not remove:
            for metal in range(topmet):
                m_hori = basic.metal_hori(w, basic.min_w['M1'], metal+1)
                m_hori = gdspy.CellReference(m_hori, (0, basic.NP_OD+0.5*(basic.OD_W-basic.min_w['M1'])))
                nwell_hori.add(m_hori)
        nwell_hori.flatten()
        return nwell_hori

    @staticmethod
    def nwell_square(dope='NP', ext=[], topmet=1): # square cell to fill corners
        # ext: extend M1 0:North, 1:East, 2:South, 3:West
        nwell_square = gdspy.Cell('NWELL_SQUARE', True)
        od_shape = gdspy.Rectangle((0,0), (basic.OD_W, basic.OD_W), basic.layer['OD'])
        nwell_square.add(od_shape)
        # Legalization changes
        delta = 0.5*(basic.OD_W-basic.min_w['M1'])
        for i in range(topmet):
            m_layer = 'M' + str(i+1)
            if len(ext) > 0:
                m1_shape = gdspy.Rectangle((0.5*(basic.OD_W-basic.min_w['M1']),0.5*(basic.OD_W-basic.min_w['M1'])), (0.5*(basic.OD_W+basic.min_w['M1']),0.5*(basic.OD_W+basic.min_w['M1'])), basic.layer[m_layer], basic.datatype[m_layer])
                nwell_square.add(m1_shape)
            m1_ext_ns = gdspy.Rectangle((0,0), (basic.min_w['M1'], delta), basic.layer[m_layer], basic.datatype[m_layer])
            m1_ext_ew = gdspy.Rectangle((0,0), (delta, basic.min_w['M1']), basic.layer[m_layer], basic.datatype[m_layer])
            if 0 in ext:
                m1_ext_1 = gdspy.copy(m1_ext_ns, delta, delta+basic.min_w['M1'])
                nwell_square.add(m1_ext_1)
            if 1 in ext:
                m1_ext_2 = gdspy.copy(m1_ext_ew, delta+basic.min_w['M1'], delta)
                nwell_square.add(m1_ext_2)
            if 2 in ext:
                m1_ext_3 = gdspy.copy(m1_ext_ns, delta, 0)
                nwell_square.add(m1_ext_3)
            if 3 in ext:
                m1_ext_4 = gdspy.copy(m1_ext_ew, 0, delta)
                nwell_square.add(m1_ext_4)
        # Removed co for DRC
        #co_en = 0.5 * (basic.OD_W - basic.min_w['CO'])
        #co_shape = gdspy.Rectangle((co_en, co_en), (co_en+basic.min_w['CO'], co_en+basic.min_w['CO']), layer['CO'])
        #nwell_square.add(co_shape)
        np_shape = gdspy.Rectangle((-basic.NP_OD, -basic.NP_OD), (basic.OD_W+basic.NP_OD, basic.OD_W+basic.NP_OD), basic.layer[dope])
        nwell_square.add(np_shape)
        nwell_square.flatten()
        return nwell_square

    @staticmethod
    def legal_well(ll, ur, origin=[0,0]):
        # Legalize ll and ur of basic.min_w well generation
        off = 0.5 * (basic.OD_W + basic.min_w['M1']) + basic.NW_OD
        ll = [ll[0] - off , ll[1] - off]
        ur = [ur[0] + off - basic.min_w['M1'], ur[1] + off - basic.min_w['M1']]
        ll[0] = basic.legal_coord(ll, origin, 1)[0] + off
        ll[1] = basic.legal_coord(ll, origin, 1)[1] + off
        ur[0] = basic.legal_coord(ur, origin, 3)[0] - off + basic.min_w['M1']
        ur[1] = basic.legal_coord(ur, origin, 3)[1] - off + basic.min_w['M1']
        return ll, ur
        
    @staticmethod
    def nwell_GR(ll, ur, origin=None, removeVert=False): 
        # ll is lower left, ur is upper right cordinate of mos PP layer
        # Default is to go up to M3
        pin = Pin('B')
        if origin:
            ll, ur = basic.legal_well(ll, ur, origin)
        nwell_cell = gdspy.Cell('NWELL_GR', True)
        width = ur[0] - ll[0] + 2 * basic.NW_OD
        height = ur[1] - ll[1] + 2 * basic.NW_OD
        cell_width = basic.NP_OD + basic.NW_OD + basic.OD_W
        nwell_vert_cell = basic.nwell_vert(height, remove=removeVert, topmet=1)
        nwell_hori_cell = basic.nwell_hori(width, remove=removeVert, topmet=1)
        nwell_hori_bot =  basic.nwell_hori(width, topmet=6)
        nwell_vert_left = basic.nwell_vert(height, remove=True, topmet=1)
        nwell_hori_1 = gdspy.CellReference(nwell_hori_bot, (ll[0]-basic.NW_OD, ll[1]-cell_width))
        nwell_hori_2 = gdspy.CellReference(nwell_hori_cell, (ll[0]-basic.NW_OD, ur[1]+basic.NW_OD-basic.NP_OD))
        nwell_vert_1 = gdspy.CellReference(nwell_vert_left, (ll[0]-cell_width, ll[1]-basic.NW_OD))
        nwell_vert_2 = gdspy.CellReference(nwell_vert_cell, (ur[0]+basic.NW_OD-basic.NP_OD, ll[1]-basic.NW_OD))
        nwell_cell.add(nwell_vert_1)
        nwell_cell.add(nwell_vert_2)
        nwell_cell.add(nwell_hori_1)
        nwell_cell.add(nwell_hori_2)
        s_dist = basic.NW_OD + basic.OD_W
        if not removeVert:
            nwell_s_1 = gdspy.CellReference(basic.nwell_square('NP',[1],6), (ll[0]-s_dist, ll[1]-s_dist))
            nwell_s_2 = gdspy.CellReference(basic.nwell_square('NP',[1],1), (ll[0]-s_dist, ur[1]+basic.NW_OD))
            nwell_s_3 = gdspy.CellReference(basic.nwell_square('NP',[0,3],6), (ur[0]+basic.NW_OD, ll[1]-s_dist))
            nwell_s_4 = gdspy.CellReference(basic.nwell_square('NP',[2,3],1), (ur[0]+basic.NW_OD, ur[1]+basic.NW_OD))
        else:
            nwell_s_1 = gdspy.CellReference(basic.nwell_square('NP',[1],6), (ll[0]-s_dist, ll[1]-s_dist))
            nwell_s_2 = gdspy.CellReference(basic.nwell_square('NP',[],1), (ll[0]-s_dist, ur[1]+basic.NW_OD))
            nwell_s_3 = gdspy.CellReference(basic.nwell_square('NP',[3],6), (ur[0]+basic.NW_OD, ll[1]-s_dist))
            nwell_s_4 = gdspy.CellReference(basic.nwell_square('NP',[],1), (ur[0]+basic.NW_OD, ur[1]+basic.NW_OD))
        nwell_cell.add(nwell_s_1)
        nwell_cell.add(nwell_s_2)
        nwell_cell.add(nwell_s_3)
        nwell_cell.add(nwell_s_4)
        cell_width = basic.OD_W + 2 * basic.NW_OD
        nw_enclose = gdspy.Rectangle((ll[0]-cell_width, ll[1]-cell_width), (ur[0]+cell_width, ur[1]+cell_width), basic.layer['NW'])
        nwell_cell.add(nw_enclose)
        nwell_cell.flatten()
        # Add pin shapes
        m1_hori_1_x_1 = ll[0]-s_dist+0.5*(basic.OD_W-basic.min_w['M1'])
        m1_hori_1_y_1 = ll[1]-s_dist+0.5*(basic.OD_W-basic.min_w['M1'])
        m1_hori_1_x_2 = ur[0]+basic.NW_OD+0.5*(basic.OD_W+basic.min_w['M1'])
        m1_hori_1_y_2 = ur[1]+basic.NW_OD+0.5*(basic.OD_W-basic.min_w['M1'])
        pin.add_shape('M6',[[m1_hori_1_x_1,m1_hori_1_y_1],[m1_hori_1_x_2,m1_hori_1_y_1+basic.min_w['M1']]])
        if not removeVert:
            pin.add_shape('M1',[[m1_hori_1_x_1,m1_hori_1_y_2],[m1_hori_1_x_2,m1_hori_1_y_2+basic.min_w['M1']]])
            #pin.add_shape('M1',[[m1_hori_1_x_1,m1_hori_1_y_1],[m1_hori_1_x_1+basic.min_w['M1'],m1_hori_1_y_2+basic.min_w['M1']]])
            pin.add_shape('M1',[[m1_hori_1_x_2-basic.min_w['M1'],m1_hori_1_y_1],[m1_hori_1_x_2,m1_hori_1_y_2+basic.min_w['M1']]])
        return nwell_cell, pin

    @staticmethod
    def sub_GR(ll, ur, origin=None, removeVert=False): 
        # ll is lower left, ur is upper right cordinate of mos PP layer
        pin = Pin('B')
        if origin:
            ll, ur = basic.legal_well(ll, ur, origin)
        nwell_cell = gdspy.Cell('SUB_GR', True)
        width = ur[0] - ll[0] + 2 * basic.NW_OD
        height = ur[1] - ll[1] + 2 * basic.NW_OD
        cell_width = basic.NP_OD + basic.NW_OD + basic.OD_W
        nwell_vert_cell = basic.nwell_vert(height, 'PP', removeVert)
        nwell_vert_left = basic.nwell_vert(height, 'PP', removeVert)
        nwell_hori_cell = basic.nwell_hori(width, 'PP', removeVert)
        nwell_hori_bot =  basic.nwell_hori(width, 'PP', False, 6)
        nwell_hori_1 = gdspy.CellReference(nwell_hori_bot, (ll[0]-basic.NW_OD, ll[1]-cell_width))
        nwell_hori_2 = gdspy.CellReference(nwell_hori_cell, (ll[0]-basic.NW_OD, ur[1]+basic.NW_OD-basic.NP_OD))
        nwell_vert_1 = gdspy.CellReference(nwell_vert_left, (ll[0]-cell_width, ll[1]-basic.NW_OD))
        nwell_vert_2 = gdspy.CellReference(nwell_vert_cell, (ur[0]+basic.NW_OD-basic.NP_OD, ll[1]-basic.NW_OD))
        nwell_cell.add(nwell_vert_1)
        nwell_cell.add(nwell_vert_2)
        nwell_cell.add(nwell_hori_1)
        nwell_cell.add(nwell_hori_2)
        s_dist = basic.NW_OD + basic.OD_W
        if not removeVert:
            nwell_s_1 = gdspy.CellReference(basic.nwell_square('PP',[0,1], 6), (ll[0]-s_dist, ll[1]-s_dist))
            nwell_s_2 = gdspy.CellReference(basic.nwell_square('PP',[1,2]), (ll[0]-s_dist, ur[1]+basic.NW_OD))
            nwell_s_3 = gdspy.CellReference(basic.nwell_square('PP',[0,3], 6), (ur[0]+basic.NW_OD, ll[1]-s_dist))
            nwell_s_4 = gdspy.CellReference(basic.nwell_square('PP',[2,3]), (ur[0]+basic.NW_OD, ur[1]+basic.NW_OD))

        else:
            nwell_s_1 = gdspy.CellReference(basic.nwell_square('PP',[1], 6), (ll[0]-s_dist, ll[1]-s_dist))
            nwell_s_2 = gdspy.CellReference(basic.nwell_square('PP',[]), (ll[0]-s_dist, ur[1]+basic.NW_OD))
            nwell_s_3 = gdspy.CellReference(basic.nwell_square('PP',[3], 6), (ur[0]+basic.NW_OD, ll[1]-s_dist))
            nwell_s_4 = gdspy.CellReference(basic.nwell_square('PP',[]), (ur[0]+basic.NW_OD, ur[1]+basic.NW_OD))
        nwell_cell.add(nwell_s_1)
        nwell_cell.add(nwell_s_2)
        nwell_cell.add(nwell_s_3)
        nwell_cell.add(nwell_s_4)
        nwell_cell.flatten()
        # Add pin shapes
        m1_hori_1_x_1 = ll[0]-s_dist+0.5*(basic.OD_W-basic.min_w['M1'])
        m1_hori_1_y_1 = ll[1]-s_dist+0.5*(basic.OD_W-basic.min_w['M1'])
        m1_hori_1_x_2 = ur[0]+basic.NW_OD+0.5*(basic.OD_W+basic.min_w['M1'])
        m1_hori_1_y_2 = ur[1]+basic.NW_OD+0.5*(basic.OD_W-basic.min_w['M1'])
        pin.add_shape('M6',[[m1_hori_1_x_1,m1_hori_1_y_1],[m1_hori_1_x_2,m1_hori_1_y_1+basic.min_w['M1']]])
        if not removeVert:
            pin.add_shape('M1',[[m1_hori_1_x_1,m1_hori_1_y_2],[m1_hori_1_x_2,m1_hori_1_y_2+basic.min_w['M1']]])
            #pin.add_shape('M1',[[m1_hori_1_x_1,m1_hori_1_y_1],[m1_hori_1_x_1+min_w['M1'],m1_hori_1_y_2+basic.min_w['M1']]])
            pin.add_shape('M1',[[m1_hori_1_x_2-basic.min_w['M1'],m1_hori_1_y_1],[m1_hori_1_x_2,m1_hori_1_y_2+basic.min_w['M1']]])
        return nwell_cell, pin

    @staticmethod
    def flip_cell(cell):
        flip_cell = gdspy.Cell(cell.name, True)
        bounding_box = cell.get_bounding_box()
        x_sym_axis = bounding_box[0][0] + bounding_box[1][0]
        # Floating point error 
        # Since gdsii precision is 5nm, here we only round to 1nm precision
        #x_sym_axis = round(x_sym_axis * 10000) / 10000.0
        polydict = cell.get_polygons(by_spec=True)
        for key in polydict:
            basic.layer, basic.datatype = key
            for shape in polydict[key]:
                x_min = shape[0][0]
                y_min = shape[0][1]
                x_max = shape[2][0]
                y_max = shape[2][1]
                x_min_s = x_sym_axis - x_max
                x_max_s = x_sym_axis - x_min
                new_shape = gdspy.Rectangle([x_min_s,y_min], [x_max_s,y_max], basic.layer, datatype=basic.datatype)
                flip_cell.add(new_shape)
        flip_cell = flip_cell.flatten()
        return flip_cell

    @staticmethod
    def check_legal_coord(coord, origin=[0,0]):
        if basic.legal(coord[0]-origin[0]) == coord[0]-origin[0] and basic.legal(coord[1]-origin[1]) == coord[1]-origin[1]:
            return True
        print(coord, origin)
        return False

    @staticmethod
    def check_pin(pin, origin=[-0.5*min_w['M1'], -0.5*min_w['M1']]):
        for shape in pin.shape:
            if not basic.check_legal_coord(shape[1], origin):
                print(shape[1], origin)
                return False
            if not basic.check_legal_coord([shape[2][0]+basic.min_w['SP'],shape[2][1]+basic.min_w['SP']], origin):
                print(shape[2], origin)
                return False
        return True

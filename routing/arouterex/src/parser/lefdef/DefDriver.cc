/**
 * @file   DefDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref DefParser::Driver
 */

#include "DefDriver.h"
#include <limbo/preprocessor/Msg.h>

namespace DefParser
{

    Driver::Driver(DefDataBase& db)
            : trace_scanning(false),
              trace_parsing(false),
              m_db(db)
    {
    }

/// =========== callbacks ============
///@{

/// a local temporary global variable 
/// refer to Driver::m_db everytime parsing a file 
/// reset to NULL once parsing finished 
    DefDataBase* defDB = NULL;
    Driver* defDriver = NULL;
    int numObjs;
    int isSumSet;      // to keep track if within SUM
    int isProp = 0;    // for PROPERTYDEFINITIONS
    int begOperand;    // to keep track for constraint, to print - as the 1st char
    double curVer = 0;
    int setSNetWireCbk = 0;
    int isSessionless = 0;
    int ignoreRowNames = 0;
    int ignoreViaNames = 0;
    int testDebugPrint = 0;
    void* userData = NULL;

    void dataError()
    {
        limboPrint(limbo::kERROR, "returned user data is not correct!\n");
    }

    void checkType(defrCallbackType_e c)
    {
        if (c >= 0 && c <= defrDesignEndCbkType)
        {
            // OK
        }
        else
        {
            limboPrint(limbo::kERROR, "callback type is out of bounds!\n");
        }
    }


    int done(defrCallbackType_e c, void*, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDB->end_def_design();
        return 0;
    }

    int endfunc(defrCallbackType_e c, void*, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        return 0;
    }


    const char* orientStr(int orient)
    {
        switch (orient)
        {
            case 0: return ((const char*)"N");
            case 1: return ((const char*)"W");
            case 2: return ((const char*)"S");
            case 3: return ((const char*)"E");
            case 4: return ((const char*)"FN");
            case 5: return ((const char*)"FW");
            case 6: return ((const char*)"FS");
            case 7: return ((const char*)"FE");
        };
        return ((const char*)"BOGUS");
    }

    int compMSL(defrCallbackType_e c, defiComponentMaskShiftLayer* co, defiUserData ud)
    {
        int i;

        checkType(c);
        if (ud != userData) dataError();

        if (co->numMaskShiftLayers())
        {
            limboPrint(limbo::kNONE, "\nCOMPONENTMASKSHIFT ");

            for (i = 0; i < co->numMaskShiftLayers(); i++)
            {
                limboPrint(limbo::kNONE, "%s ", co->maskShiftLayer(i));
            }
            limboPrint(limbo::kNONE, ";\n");
        }

        return 0;
    }

    int compf(defrCallbackType_e c, defiComponent* co, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDriver->component().comp_name = co->id(); // instance name
        defDriver->component().macro_name = co->name(); // cell type
        if (co->isFixed())
            defDriver->component().status = "FIXED";
        if (co->isPlaced())
            defDriver->component().status = "PLACED";
        if (co->isCover())
            defDriver->component().status = "COVER";
        if (co->isUnplaced())
            defDriver->component().status = "UNPLACED";
        if (co->hasRegionName() && co->hasRegionBounds())
            defDriver->component().ifregion = true;
        else
            defDriver->component().ifregion = false;
        if (co->hasRegionName()) {
            defDriver->component().regionName = co->regionName();
        }
        if (co->hasRegionBounds()) {
            int *xl, *yl, *xh, *yh;
            int size;
            co->defiComponent::regionBounds(&size, &xl, &yl, &xh, &yh);
            defDriver->component().region_size = size;
            for (int i = 0; i < size; i++) {
                defDriver->component().xl_r = xl;
                defDriver->component().yl_r = yl;
                defDriver->component().xh_r = xh;
                defDriver->component().yh_r = yh;

            }
        }
        defDriver->component().origin[0] = co->placementX();
        defDriver->component().origin[1] = co->placementY();
        defDriver->component().orient = orientStr(co->placementOrient());

        defDB->add_def_component(defDriver->component());
        defDriver->component().reset();

        return 0;
    }


    int netpath(defrCallbackType_e, defiNet*, defiUserData)
    {
        limboPrint(limbo::kNONE, "\n");

        limboPrint(limbo::kNONE, "Callback of partial path for net\n");

        return 0;
    }


    int netNamef(defrCallbackType_e c, const char* netName, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDriver->net().net_name = netName;
        return 0;
    }

    int subnetNamef(defrCallbackType_e c, const char* subnetName, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        if (curVer >= 5.6)
            limboPrint(limbo::kNONE, "   + SUBNET CBK %s ", subnetName);
        return 0;
    }

    int nondefRulef(defrCallbackType_e c, const char* ruleName, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        if (curVer >= 5.6)
            limboPrint(limbo::kNONE, "   + NONDEFAULTRULE CBK %s ", ruleName);
        return 0;
    }

    int netf(defrCallbackType_e c, defiNet* net, defiUserData ud)
    {
        // For net and special net.
        checkType(c);
        if (ud != userData) dataError();
        if (c != defrNetCbkType)
            limboPrint(limbo::kNONE, "BOGUS NET TYPE  ");
        // 5/6/2004 - don't need since I have a callback for the name
        //  else
        //      limboPrint(limbo::kNONE, "- %s ", net->name());

        //  net->changeNetName("newNetName");
        //  limboPrint(limbo::kNONE, "%s ", net->name());
        defDriver->net().vNetPin.resize(net->numConnections());
        // compName & pinName
        for (int i = 0; i < net->numConnections(); i++)
            defDriver->net().vNetPin[i] = std::make_pair(std::string(net->instance(i)), std::string(net->pin(i)));

        defDB->add_def_net(defDriver->net());
        defDriver->net().reset();
        return 0;
    }


    int snetpath(defrCallbackType_e c, defiNet* ppath, defiUserData ud)
    {
        int         i, j, x, y, z, count, newLayer;
        char*       layerName;
        double      dist, left, right;
        defiPath*   p;
        defiSubnet  *s;
        int         path;
        defiShield* shield;
        defiWire*   wire;
        int         numX, numY, stepX, stepY;

        if (c != defrSNetPartialPathCbkType)
            return 1;
        if (ud != userData) dataError();

        limboPrint(limbo::kNONE, "SPECIALNET partial data\n");

        //limboPrint(limbo::kNONE, "- %s ", ppath->name());
        defDriver->snetp().name = ppath->name();
        cout<<"path"<<ppath->name()<<endl;

        count = 0;
        // compName & pinName
        for (i = 0; i < ppath->numConnections(); i++)
        {
            // set the limit of only 5 items print out in one line
            count++;
            if (count >= 5)
            {
                //limboPrint(limbo::kNONE, "\n");
                count = 0;
            }
            limboPrint(limbo::kNONE, "( %s %s ) ", ppath->instance(i),
                       ppath->pin(i));
            if (ppath->pinIsSynthesized(i))
                limboPrint(limbo::kNONE, "+ SYNTHESIZED ");
        }

        // specialWiring
        // POLYGON
        if (ppath->numPolygons())
        {
            struct defiPoints points;
            for (i = 0; i < ppath->numPolygons(); i++)
            {
                limboPrint(limbo::kNONE, "\n  + POLYGON %s ", ppath->polygonName(i));
                points = ppath->getPolygon(i);
                for (j = 0; j < points.numPoints; j++)
                    limboPrint(limbo::kNONE, "%d %d ", points.x[j], points.y[j]);
            }
        }
        // RECT
        if (ppath->numRectangles())
        {
            for (i = 0; i < ppath->numRectangles(); i++)
            {
                //limboPrint(limbo::kNONE, "\n  + RECT %s %d %d %d %d", ppath->rectName(i),
                //       ppath->xl(i), ppath->yl(i),
                //      ppath->xh(i), ppath->yh(i));
            }
        }

        // COVER, FIXED, ROUTED or SHIELD
        if (ppath->numWires())
        {

            newLayer = 0;
            for (i = 0; i < ppath->numWires(); i++)
            {
                newLayer = 0;
                wire = ppath->wire(i);
                //limboPrint(limbo::kNONE, "\n  + %s ", wire->wireType());
                defDriver->snetp().wiretype = wire->wireType();
                if (strcmp (wire->wireType(), "SHIELD") == 0)
                    limboPrint(limbo::kNONE, "%s ", wire->wireShieldNetName());
                for (j = 0; j < wire->numPaths(); j++)
                {
                    string layer_name = "";
                    string via_name = "";
                    string shape_name = "";
                    bool is_via = false;
                    bool is_shape = false;
                    int x_d = 0;
                    int y_d = 0;
                    int x_dh = 0;
                    int y_dh = 0;
                    int point = 0;
                    int pathwidth = 0;
                    p = wire->path(j);
                    p->initTraverse();
                    while ((path = (int)p->next()) != DEFIPATH_DONE)
                    {
                        count++;
                        // Don't want the line to be too long
                        if (count >= 5)
                        {
                            //limboPrint(limbo::kNONE, "\n");
                            count = 0;
                        }
                        switch (path)
                        {
                            case DEFIPATH_LAYER:
                                if (newLayer == 0)
                                {
                                    //limboPrint(limbo::kNONE, "%s ", p->getLayer());
                                    layer_name = std::string( p->getLayer());
                                    newLayer = 1;
                                } else {
                                    //limboPrint(limbo::kNONE, "NEW %s ", p->getLayer());
                                    layer_name = std::string(p->getLayer());
                                }
                                break;
                            case DEFIPATH_VIA:
                                limboPrint(limbo::kNONE, "DEFIPATH_VIA %s ", ignoreViaNames ? "XXX" : p->getVia());
                                via_name = p->getVia();
                                is_via = true;
                                break;
                            case DEFIPATH_VIAROTATION:
                                limboPrint(limbo::kNONE, "DEFIPATH_VIAROTATION %s ",
                                           orientStr(p->getViaRotation()));
                                break;
                            case DEFIPATH_VIADATA:
                                p->getViaData(&numX, &numY, &stepX, &stepY);
                                limboPrint(limbo::kNONE, "DEFIPATH_VIADATA DO %d BY %d STEP %d %d ", numX, numY,
                                           stepX, stepY);
                                break;
                            case DEFIPATH_WIDTH:
                                //limboPrint(limbo::kNONE, "DEFIPATH_WIDTH %d ", p->getWidth());
                                pathwidth = p->getWidth();
                                break;
                            case DEFIPATH_MASK:
                                limboPrint(limbo::kNONE, "DEFIPATH_MASK MASK %d ", p->getMask());
                                break;
                            case DEFIPATH_VIAMASK:
                                limboPrint(limbo::kNONE, "DEFIPATH_VIAMASK MASK %d%d%d ",
                                           p->getViaTopMask(),
                                           p->getViaCutMask(),
                                           p->getViaBottomMask());
                                break;
                            case DEFIPATH_POINT:
                                p->getPoint(&x, &y);
                                //limboPrint(limbo::kNONE, "( %d %d ) ", x, y);
                                if(point == 0) {
                                    x_d = x;
                                    y_d = y;
                                    point = 1;
                                }
                                else {
                                    x_dh = x;
                                    y_dh = y;
                                    point = 2;
                                }
                                break;
                            case DEFIPATH_FLUSHPOINT:
                                p->getFlushPoint(&x, &y, &z);
                                limboPrint(limbo::kNONE, "( %d %d %d ) ", x, y, z);
                                break;
                            case DEFIPATH_TAPER:
                                limboPrint(limbo::kNONE, "TAPER ");
                                break;
                            case DEFIPATH_SHAPE:
                                //limboPrint(limbo::kNONE, "+ SHAPE %s ", p->getShape());
                                shape_name = std::string(p->getShape());
                                is_shape = true;
                                break;
                            case DEFIPATH_STYLE:
                                limboPrint(limbo::kNONE, "+ STYLE %d ", p->getStyle());
                                break;
                        }
                    }
                    /// End of a path, push the data into the defDriver->snetp()
                    defDriver->snetp().layers.push_back(layer_name);
                    defDriver->snetp().pathwidth.push_back(pathwidth);
                    if (point == 2) {
                        defDriver->snetp().xl.push_back(x_d);
                        defDriver->snetp().yl.push_back(y_d);
                        defDriver->snetp().yh.push_back(y_dh);
                        defDriver->snetp().xh.push_back(x_dh);
                    }
                    else if (point == 1) {
                        defDriver->snetp().xl.push_back(x_d);
                        defDriver->snetp().yl.push_back(y_d);
                        defDriver->snetp().yh.push_back(-1);
                        defDriver->snetp().xh.push_back(-1);
                    }
                    else if (point == 0) {
                        defDriver->snetp().xl.push_back(-1);
                        defDriver->snetp().yl.push_back(-1);
                        defDriver->snetp().yh.push_back(-1);
                        defDriver->snetp().xh.push_back(-1);
                    }
                    if (is_shape) {

                        defDriver->snetp().compname.push_back(shape_name);
                        defDriver->snetp().isshape.push_back(true);


                    }
                    else{
                        defDriver->snetp().compname.push_back("undefined");
                        defDriver->snetp().isshape.push_back(false);
                    }

                    if (is_via) {

                        //defDriver->snetp().compname.push_back(via_name);
                        defDriver->snetp().isvia.push_back(true);
                        defDriver->snetp().vianame.push_back(via_name);

                    }
                    else {
                        defDriver->snetp().vianame.push_back("undefined");
                        defDriver->snetp().isvia.push_back(false);
                    }

                }
                limboPrint(limbo::kNONE, "\n");
                count = 0;
            }
        }

        if (ppath->hasSubnets())
        {
            for (i = 0; i < ppath->numSubnets(); i++)
            {
                s = ppath->subnet(i);
                if (s->numConnections())
                {
                    if (s->pinIsMustJoin(0))
                        limboPrint(limbo::kNONE, "- MUSTJOIN ");
                    else
                        limboPrint(limbo::kNONE, "- %s ", s->name());
                    for (j = 0; j < s->numConnections(); j++)
                    {
                        limboPrint(limbo::kNONE, " ( %s %s )\n", s->instance(j),
                                   s->pin(j));
                    }
                }

                // regularWiring
                if (s->numWires())
                {
                    for (i = 0; i < s->numWires(); i++)
                    {
                        wire = s->wire(i);
                        limboPrint(limbo::kNONE, "  + %s ", wire->wireType());
                        for (j = 0; j < wire->numPaths(); j++)
                        {
                            p = wire->path(j);
                            p->print(stdout);
                        }
                    }
                }
            }
        }

        if (ppath->numProps())
        {
            for (i = 0; i < ppath->numProps(); i++)
            {
                if (ppath->propIsString(i))
                    limboPrint(limbo::kNONE, "  + PROPERTY %s %s ", ppath->propName(i),
                               ppath->propValue(i));
                if (ppath->propIsNumber(i))
                    limboPrint(limbo::kNONE, "  + PROPERTY %s %g ", ppath->propName(i),
                               ppath->propNumber(i));
                switch (ppath->propType(i))
                {
                    case 'R': limboPrint(limbo::kNONE, "REAL ");
                        break;
                    case 'I': limboPrint(limbo::kNONE, "INTEGER ");
                        break;
                    case 'S': limboPrint(limbo::kNONE, "STRING ");
                        break;
                    case 'Q': limboPrint(limbo::kNONE, "QUOTESTRING ");
                        break;
                    case 'N': limboPrint(limbo::kNONE, "NUMBER ");
                        break;
                }
                limboPrint(limbo::kNONE, "\n");
            }
        }

        // SHIELD
        count = 0;
        // testing the SHIELD for 5.3, obsolete in 5.4
        if (ppath->numShields())
        {
            for (i = 0; i < ppath->numShields(); i++)
            {
                shield = ppath->shield(i);
                limboPrint(limbo::kNONE, "\n  + SHIELD %s ", shield->shieldName());
                newLayer = 0;
                for (j = 0; j < shield->numPaths(); j++)
                {
                    p = shield->path(j);
                    p->initTraverse();
                    while ((path = (int)p->next()) != DEFIPATH_DONE)
                    {
                        count++;
                        // Don't want the line to be too long
                        if (count >= 5)
                        {
                            limboPrint(limbo::kNONE, "\n");
                            count = 0;
                        }
                        switch (path)
                        {
                            case DEFIPATH_LAYER:
                                if (newLayer == 0)
                                {
                                    limboPrint(limbo::kNONE, "%s ", p->getLayer());
                                    newLayer = 1;
                                } else
                                    limboPrint(limbo::kNONE, "NEW %s ", p->getLayer());
                                break;
                            case DEFIPATH_VIA:
                                limboPrint(limbo::kNONE, "%s ", ignoreViaNames ? "XXX" : p->getVia());
                                break;
                            case DEFIPATH_VIAROTATION:
                                if (newLayer)
                                    limboPrint(limbo::kNONE, "%s ",
                                               orientStr(p->getViaRotation()));
                                else
                                    limboPrint(limbo::kNONE, "Str %s ",
                                               p->getViaRotationStr());
                                break;
                            case DEFIPATH_WIDTH:
                                limboPrint(limbo::kNONE, "%d ", p->getWidth());
                                break;
                            case DEFIPATH_MASK:
                                limboPrint(limbo::kNONE, "MASK %d ", p->getMask());
                                break;
                            case DEFIPATH_VIAMASK:
                                limboPrint(limbo::kNONE, "MASK %d%d%d ",
                                           p->getViaTopMask(),
                                           p->getViaCutMask(),
                                           p->getViaBottomMask());
                                break;
                            case DEFIPATH_POINT:
                                p->getPoint(&x, &y);
                                limboPrint(limbo::kNONE, "( %d %d ) ", x, y);
                                break;
                            case DEFIPATH_FLUSHPOINT:
                                p->getFlushPoint(&x, &y, &z);
                                limboPrint(limbo::kNONE, "( %d %d %d ) ", x, y, z);
                                break;
                            case DEFIPATH_TAPER:
                                limboPrint(limbo::kNONE, "TAPER ");
                                break;
                            case DEFIPATH_SHAPE:
                                limboPrint(limbo::kNONE, "+ SHAPE %s ", p->getShape());
                                break;
                            case DEFIPATH_STYLE:
                                limboPrint(limbo::kNONE, "+ STYLE %d ", p->getStyle());
                        }
                    }
                }
            }
        }

        // layerName width
        if (ppath->hasWidthRules())
        {
            for (i = 0; i < ppath->numWidthRules(); i++)
            {
                ppath->widthRule(i, &layerName, &dist);
                limboPrint(limbo::kNONE, "\n  + WIDTH %s %g ", layerName, dist);
            }
        }

        // layerName spacing
        if (ppath->hasSpacingRules())
        {
            for (i = 0; i < ppath->numSpacingRules(); i++)
            {
                ppath->spacingRule(i, &layerName, &dist, &left, &right);
                if (left == right)
                    limboPrint(limbo::kNONE, "\n  + SPACING %s %g ", layerName, dist);
                else
                    limboPrint(limbo::kNONE, "\n  + SPACING %s %g RANGE %g %g ",
                               layerName, dist, left, right);
            }
        }

        if (ppath->hasFixedbump())
            limboPrint(limbo::kNONE, "\n  + FIXEDBUMP ");
        if (ppath->hasFrequency())
            limboPrint(limbo::kNONE, "\n  + FREQUENCY %g ", ppath->frequency());
        if (ppath->hasVoltage())
            limboPrint(limbo::kNONE, "\n  + VOLTAGE %g ", ppath->voltage());
        if (ppath->hasWeight())
            limboPrint(limbo::kNONE, "\n  + WEIGHT %d ", ppath->weight());
        if (ppath->hasCap())
            limboPrint(limbo::kNONE, "\n  + ESTCAP %g ", ppath->cap());
        if (ppath->hasSource())
            limboPrint(limbo::kNONE, "\n  + SOURCE %s ", ppath->source());
        if (ppath->hasPattern())
            limboPrint(limbo::kNONE, "\n  + PATTERN %s ", ppath->pattern());
        if (ppath->hasOriginal())
            limboPrint(limbo::kNONE, "\n  + ORIGINAL %s ", ppath->original());
        if (ppath->hasUse())
            limboPrint(limbo::kNONE, "\n  + USE %s ", ppath->use());

        //limboPrint(limbo::kNONE, "\n");
        defDB->add_def_snetpath(defDriver->snetp());
        defDriver->snetp().reset();

        return 0;
    }


    int snetwire(defrCallbackType_e c, defiNet* ppath, defiUserData ud)
    {
        int         i, j, x, y, z, count = 0, newLayer;
        defiPath*   p;
        int         path;
        defiWire*   wire;
        defiShield* shield;
        int         numX, numY, stepX, stepY;

        if (c != defrSNetWireCbkType)
            return 1;
        if (ud != userData) dataError();

        limboPrint(limbo::kNONE, "SPECIALNET wire data\n");

        limboPrint(limbo::kNONE, "- %s ", ppath->name());

        // POLYGON
        if (ppath->numPolygons())
        {
            struct defiPoints points;
            for (i = 0; i < ppath->numPolygons(); i++)
            {
                limboPrint(limbo::kNONE, "\n  + POLYGON %s ", ppath->polygonName(i));

                points = ppath->getPolygon(i);

                for (j = 0; j < points.numPoints; j++)
                {
                    limboPrint(limbo::kNONE, "%d %d ", points.x[j], points.y[j]);
                }
            }
            // RECT
        }
        if (ppath->numRectangles())
        {
            for (i = 0; i < ppath->numRectangles(); i++)
            {
                limboPrint(limbo::kNONE, "\n  + RECT %s %d %d %d %d", ppath->rectName(i),
                           ppath->xl(i), ppath->yl(i),
                           ppath->xh(i), ppath->yh(i));
            }
        }
        // VIA
        if (ppath->numViaSpecs())
        {
            for (i = 0; i < ppath->numViaSpecs(); i++)
            {
                limboPrint(limbo::kNONE, "\n  + VIA %s ", ppath->viaName(i)),
                        limboPrint(limbo::kNONE, " %s", ppath->viaOrientStr(i));

                defiPoints points = ppath->getViaPts(i);

                for (int j = 0; j < points.numPoints; j++)
                {
                    limboPrint(limbo::kNONE, " %d %d", points.x[j], points.y[j]);
                }
            }
        }

        // specialWiring
        if (ppath->numWires())
        {
            newLayer = 0;
            for (i = 0; i < ppath->numWires(); i++)
            {
                newLayer = 0;
                wire = ppath->wire(i);
                limboPrint(limbo::kNONE, "\n  + %s ", wire->wireType());
                if (strcmp (wire->wireType(), "SHIELD") == 0)
                    limboPrint(limbo::kNONE, "%s ", wire->wireShieldNetName());
                for (j = 0; j < wire->numPaths(); j++)
                {
                    p = wire->path(j);
                    p->initTraverse();
                    while ((path = (int)p->next()) != DEFIPATH_DONE)
                    {
                        count++;
                        // Don't want the line to be too long
                        if (count >= 5)
                        {
                            limboPrint(limbo::kNONE, "\n");
                            count = 0;
                        }
                        switch (path)
                        {
                            case DEFIPATH_LAYER:
                                if (newLayer == 0)
                                {
                                    limboPrint(limbo::kNONE, "%s ", p->getLayer());
                                    newLayer = 1;
                                } else
                                    limboPrint(limbo::kNONE, "NEW %s ", p->getLayer());
                                break;
                            case DEFIPATH_VIA:
                                limboPrint(limbo::kNONE, "%s ", ignoreViaNames ? "XXX" : p->getVia());
                                break;
                            case DEFIPATH_VIAROTATION:
                                limboPrint(limbo::kNONE, "%s ",
                                           orientStr(p->getViaRotation()));
                                break;
                            case DEFIPATH_VIADATA:
                                p->getViaData(&numX, &numY, &stepX, &stepY);
                                limboPrint(limbo::kNONE, "DO %d BY %d STEP %d %d ", numX, numY,
                                           stepX, stepY);
                                break;
                            case DEFIPATH_WIDTH:
                                limboPrint(limbo::kNONE, "%d ", p->getWidth());
                                break;
                            case DEFIPATH_MASK:
                                limboPrint(limbo::kNONE, "MASK %d ", p->getMask());
                                break;
                            case DEFIPATH_VIAMASK:
                                limboPrint(limbo::kNONE, "MASK %d%d%d ",
                                           p->getViaTopMask(),
                                           p->getViaCutMask(),
                                           p->getViaBottomMask());
                                break;
                            case DEFIPATH_POINT:
                                p->getPoint(&x, &y);
                                limboPrint(limbo::kNONE, "( %d %d ) ", x, y);
                                break;
                            case DEFIPATH_FLUSHPOINT:
                                p->getFlushPoint(&x, &y, &z);
                                limboPrint(limbo::kNONE, "( %d %d %d ) ", x, y, z);
                                break;
                            case DEFIPATH_TAPER:
                                limboPrint(limbo::kNONE, "TAPER ");
                                break;
                            case DEFIPATH_SHAPE:
                                limboPrint(limbo::kNONE, "+ SHAPE %s ", p->getShape());
                                break;
                            case DEFIPATH_STYLE:
                                limboPrint(limbo::kNONE, "+ STYLE %d ", p->getStyle());
                                break;
                        }
                    }
                }
                limboPrint(limbo::kNONE, "\n");
                count = 0;
            }
        } else if (ppath->numShields())
        {
            for (i = 0; i < ppath->numShields(); i++)
            {
                shield = ppath->shield(i);
                limboPrint(limbo::kNONE, "\n  + SHIELD %s ", shield->shieldName());
                newLayer = 0;
                for (j = 0; j < shield->numPaths(); j++)
                {
                    p = shield->path(j);
                    p->initTraverse();
                    while ((path = (int)p->next()) != DEFIPATH_DONE)
                    {
                        count++;
                        // Don't want the line to be too long
                        if (count >= 5)
                        {
                            limboPrint(limbo::kNONE, "\n");
                            count = 0;
                        }
                        switch (path)
                        {
                            case DEFIPATH_LAYER:
                                if (newLayer == 0)
                                {
                                    limboPrint(limbo::kNONE, "%s ", p->getLayer());
                                    newLayer = 1;
                                } else
                                    limboPrint(limbo::kNONE, "NEW %s ", p->getLayer());
                                break;
                            case DEFIPATH_VIA:
                                limboPrint(limbo::kNONE, "%s ", ignoreViaNames ? "XXX" : p->getVia());
                                break;
                            case DEFIPATH_VIAROTATION:
                                limboPrint(limbo::kNONE, "%s ",
                                           orientStr(p->getViaRotation()));
                                break;
                            case DEFIPATH_WIDTH:
                                limboPrint(limbo::kNONE, "%d ", p->getWidth());
                                break;
                            case DEFIPATH_MASK:
                                limboPrint(limbo::kNONE, "MASK %d ", p->getMask());
                                break;
                            case DEFIPATH_VIAMASK:
                                limboPrint(limbo::kNONE, "MASK %d%d%d ",
                                           p->getViaTopMask(),
                                           p->getViaCutMask(),
                                           p->getViaBottomMask());
                                break;
                            case DEFIPATH_POINT:
                                p->getPoint(&x, &y);
                                limboPrint(limbo::kNONE, "( %d %d ) ", x, y);
                                break;
                            case DEFIPATH_FLUSHPOINT:
                                p->getFlushPoint(&x, &y, &z);
                                limboPrint(limbo::kNONE, "( %d %d %d ) ", x, y, z);
                                break;
                            case DEFIPATH_TAPER:
                                limboPrint(limbo::kNONE, "TAPER ");
                                break;
                            case DEFIPATH_SHAPE:
                                limboPrint(limbo::kNONE, "+ SHAPE %s ", p->getShape());
                                break;
                            case DEFIPATH_STYLE:
                                limboPrint(limbo::kNONE, "+ STYLE %d ", p->getStyle());
                                break;
                        }
                    }
                }
            }
        }

        limboPrint(limbo::kNONE, "\n");

        return 0;
    }

    int snetf(defrCallbackType_e c, defiNet* net, defiUserData ud)
    {
        // For net and special net.
        int         i, j, x, y, z, count, newLayer;
        char*       layerName;
        double      dist, left, right;
        defiPath*   p;
        defiSubnet  *s;
        int         path;
        defiShield* shield;
        defiWire*   wire;
        int         numX, numY, stepX, stepY;

        checkType(c);
        if (ud != userData) dataError();
        if (c != defrSNetCbkType)
            limboPrint(limbo::kNONE, "BOGUS NET TYPE  ");

        limboPrint(limbo::kNONE, "SPECIALNET snetf\n");
        // 5/6/2004 - don't need since I have a callback for the name
        //  limboPrint(limbo::kNONE, "- %s ", net->name());

        defDriver->snetp().name = net->name();
        cout<<"name: " <<net->name();

        count = 0;
        // compName & pinName
        for (i = 0; i < net->numConnections(); i++)
        {
            // set the limit of only 5 items print out in one line
            count++;
            if (count >= 5)
            {
                //limboPrint(limbo::kNONE, "\n");
                count = 0;
            }
            limboPrint(limbo::kNONE, "( %s %s ) ", net->instance(i),
                       net->pin(i));
            if (net->pinIsSynthesized(i))
                limboPrint(limbo::kNONE, "+ SYNTHESIZED ");
        }

        // specialWiring
        if (net->numWires())
        {
            newLayer = 0;
            for (i = 0; i < net->numWires(); i++)
            {
                newLayer = 0;
                wire = net->wire(i);
                //limboPrint(limbo::kNONE, "\n  + %s ", wire->wireType());
                defDriver->snetp().wiretype = wire->wireType();
                if (strcmp (wire->wireType(), "SHIELD") == 0)
                    limboPrint(limbo::kNONE, "%s ", wire->wireShieldNetName());
                for (j = 0; j < wire->numPaths(); j++)
                {
                    /// Initialize each data for each path
                    string layer_name = "";
                    string via_name = "";
                    string shape_name = "";
                    bool is_via = false;
                    bool is_shape = false;
                    int x_d = 0;
                    int y_d = 0;
                    int x_dh = 0;
                    int y_dh = 0;
                    int point = 0;
                    int pathwidth = 0;

                    p = wire->path(j);
                    p->initTraverse();
                    if (testDebugPrint)
                    {
                        p->print(stdout);
                    } else
                    {
                        while ((path = (int)p->next()) != DEFIPATH_DONE)
                        {
                            count++;
                            // Don't want the line to be too long
                            if (count >= 5)
                            {
                                //limboPrint(limbo::kNONE, "\n");
                                count = 0;
                            }
                            switch (path)
                            {
                                case DEFIPATH_LAYER:
                                    if (newLayer == 0)
                                    {
                                        //limboPrint(limbo::kNONE, " DEFIPATH_LAYER %s ", p->getLayer());
                                        layer_name = std::string( p->getLayer());
                                        newLayer = 1;
                                    } else {
                                        //limboPrint(limbo::kNONE, " DEFIPATH_LAYER NEW %s ", p->getLayer());
                                        layer_name = std::string( p->getLayer());
                                    }
                                    break;
                                case DEFIPATH_VIA:
                                    //limboPrint(limbo::kNONE, " DEFIPATH_VIA%s ", ignoreViaNames ? "XXX" : p->getVia());
                                    via_name = p->getVia();
                                    is_via = true;
                                    break;
                                case DEFIPATH_VIAROTATION:
                                    limboPrint(limbo::kNONE, "DEFIPATH_VIAROTATION %s ",
                                               orientStr(p->getViaRotation()));
                                    break;
                                case DEFIPATH_VIADATA:
                                    p->getViaData(&numX, &numY, &stepX, &stepY);
                                    limboPrint(limbo::kNONE, "DO %d BY %d STEP %d %d ", numX, numY,
                                               stepX, stepY);
                                    break;
                                case DEFIPATH_WIDTH:
                                    //limboPrint(limbo::kNONE, "DEFIPATH_WIDTH %d ", p->getWidth());
                                    pathwidth = p->getWidth();
                                    break;
                                case DEFIPATH_MASK:
                                    limboPrint(limbo::kNONE, "MASK %d ", p->getMask());
                                    break;
                                case DEFIPATH_VIAMASK:
                                    limboPrint(limbo::kNONE, "MASK %d%d%d ",
                                               p->getViaTopMask(),
                                               p->getViaCutMask(),
                                               p->getViaBottomMask());
                                    break;
                                case DEFIPATH_POINT:
                                    p->getPoint(&x, &y);
                                    //limboPrint(limbo::kNONE, "( %d %d ) ", x, y);
                                    if(point == 0) {
                                        x_d = x;
                                        y_d = y;
                                        point = 1;
                                    }
                                    else {
                                        x_dh = x;
                                        y_dh = y;
                                        point = 2;
                                    }
                                    break;
                                case DEFIPATH_FLUSHPOINT:
                                    p->getFlushPoint(&x, &y, &z);
                                    limboPrint(limbo::kNONE, "( %d %d %d ) ", x, y, z);
                                    break;
                                case DEFIPATH_TAPER:
                                    limboPrint(limbo::kNONE, "TAPER ");
                                    break;
                                case DEFIPATH_SHAPE:
                                    //limboPrint(limbo::kNONE, "+ SHAPE %s ", p->getShape());
                                    shape_name = std::string(p->getShape());
                                    is_shape = true;
                                    break;
                                case DEFIPATH_STYLE:
                                    limboPrint(limbo::kNONE, "+ STYLE %d ", p->getStyle());
                                    break;
                            }
                        }
                        /// End of a path, push the data into the defDriver->snetp()
                        defDriver->snetp().layers.push_back(layer_name);
                        defDriver->snetp().pathwidth.push_back(pathwidth);
                        if (point == 2) {
                            defDriver->snetp().xl.push_back(x_d);
                            defDriver->snetp().yl.push_back(y_d);
                            defDriver->snetp().yh.push_back(y_dh);
                            defDriver->snetp().xh.push_back(x_dh);
                        }
                        else if (point == 1) {
                            defDriver->snetp().xl.push_back(x_d);
                            defDriver->snetp().yl.push_back(y_d);
                            defDriver->snetp().yh.push_back(-1);
                            defDriver->snetp().xh.push_back(-1);
                        }
                        else if (point == 0) {
                            defDriver->snetp().xl.push_back(-1);
                            defDriver->snetp().yl.push_back(-1);
                            defDriver->snetp().yh.push_back(-1);
                            defDriver->snetp().xh.push_back(-1);
                        }
                        if (is_shape) {

                            defDriver->snetp().compname.push_back(shape_name);
                            defDriver->snetp().isshape.push_back(true);


                        }
                        else{
                            defDriver->snetp().compname.push_back("undefined");
                            defDriver->snetp().isshape.push_back(false);
                        }

                        if (is_via) {

                            //defDriver->snetp().compname.push_back(via_name);
                            defDriver->snetp().isvia.push_back(true);
                            defDriver->snetp().vianame.push_back(via_name);

                        }
                        else {
                            defDriver->snetp().vianame.push_back("undefined");
                            defDriver->snetp().isvia.push_back(false);
                        }
                    }
                }
                // limboPrint(limbo::kNONE, "\n");
                count = 0;
            }
        }

        // POLYGON
        if (net->numPolygons())
        {
            struct defiPoints points;

            for (i = 0; i < net->numPolygons(); i++)
            {
                if (curVer >= 5.8 )
                {
                    if (strcmp(net->polyRouteStatus(i), "") != 0)
                    {
                        limboPrint(limbo::kNONE, "\n  + %s ", net->polyRouteStatus(i));
                        if (strcmp(net->polyRouteStatus(i), "SHIELD") == 0)
                        {
                            limboPrint(limbo::kNONE, "\n  + %s ", net->polyRouteStatusShieldName(i));
                        }
                    }
                    if (strcmp(net->polyShapeType(i), "") != 0)
                    {
                        limboPrint(limbo::kNONE, "\n  + SHAPE %s ", net->polyShapeType(i));
                    }
                }
                if (net->polyMask(i))
                {
                    limboPrint(limbo::kNONE, "\n  + MASK %d + POLYGON % s ", net->polyMask(i),
                               net->polygonName(i));
                } else
                {
                    limboPrint(limbo::kNONE, "\n  + POLYGON %s ", net->polygonName(i));
                }
                points = net->getPolygon(i);
                for (j = 0; j < points.numPoints; j++)
                    limboPrint(limbo::kNONE, "%d %d ", points.x[j], points.y[j]);
            }
        }
        // RECT
        if(0) {
            //if (net->numRectangles()) {}


            for (i = 0; i < net->numRectangles(); i++)
            {
                if (curVer >= 5.8 )
                {
                    if (strcmp(net->rectRouteStatus(i), "") != 0)
                    {
                        limboPrint(limbo::kNONE, "\n  + %s ", net->rectRouteStatus(i));
                        if (strcmp(net->rectRouteStatus(i), "SHIELD") == 0)
                        {
                            limboPrint(limbo::kNONE, "\n  + %s ", net->rectRouteStatusShieldName(i));
                        }
                    }
                    if (strcmp(net->rectShapeType(i), "") != 0)
                    {
                        limboPrint(limbo::kNONE, "\n  + SHAPE %s ", net->rectShapeType(i));
                    }
                }
                if (net->rectMask(i))
                {
                    limboPrint(limbo::kNONE, "\n  + MASK %d + RECT %s %d %d %d %d",
                               net->rectMask(i), net->rectName(i),
                               net->xl(i), net->yl(i), net->xh(i),
                               net->yh(i));
                } else
                {
                    limboPrint(limbo::kNONE, "\n  + RECT %s %d %d %d %d",
                               net->rectName(i),
                               net->xl(i),
                               net->yl(i),
                               net->xh(i),
                               net->yh(i));
                }
            }
        }
        // VIA
        if (curVer >= 5.8 && net->numViaSpecs())
        {
            for (i = 0; i < net->numViaSpecs(); i++)
            {
                if (strcmp(net->viaRouteStatus(i), "") != 0)
                {
                    limboPrint(limbo::kNONE, "\n  + %s ", net->viaRouteStatus(i));
                    if (strcmp(net->viaRouteStatus(i), "SHIELD") == 0)
                    {
                        limboPrint(limbo::kNONE, "\n  + %s ", net->viaRouteStatusShieldName(i));
                    }
                }
                if (strcmp(net->viaShapeType(i), "") != 0)
                {
                    limboPrint(limbo::kNONE, "\n  + SHAPE %s ", net->viaShapeType(i));
                }
                if (net->topMaskNum(i) || net->cutMaskNum(i) || net->bottomMaskNum(i))
                {
                    limboPrint(limbo::kNONE, "\n  + MASK %d%d%d + VIA %s ", net->topMaskNum(i),
                               net->cutMaskNum(i),
                               net->bottomMaskNum(i),
                               net->viaName(i));
                } else
                {
                    limboPrint(limbo::kNONE, "\n  + VIA %s ", net->viaName(i));
                }
                limboPrint(limbo::kNONE, " %s", net->viaOrientStr(i));

                defiPoints points = net->getViaPts(i);

                for (int j = 0; j < points.numPoints; j++)
                {
                    limboPrint(limbo::kNONE, " %d %d", points.x[j], points.y[j]);
                }
                limboPrint(limbo::kNONE, ";\n");

            }
        }

        if (net->hasSubnets())
        {
            for (i = 0; i < net->numSubnets(); i++)
            {
                s = net->subnet(i);
                if (s->numConnections())
                {
                    if (s->pinIsMustJoin(0))
                        limboPrint(limbo::kNONE, "- MUSTJOIN ");
                    else
                        limboPrint(limbo::kNONE, "- %s ", s->name());
                    for (j = 0; j < s->numConnections(); j++)
                    {
                        limboPrint(limbo::kNONE, " ( %s %s )\n", s->instance(j),
                                   s->pin(j));
                    }
                }

                // regularWiring
                if (s->numWires())
                {
                    for (i = 0; i < s->numWires(); i++)
                    {
                        wire = s->wire(i);
                        limboPrint(limbo::kNONE, "  + %s ", wire->wireType());
                        for (j = 0; j < wire->numPaths(); j++)
                        {
                            p = wire->path(j);
                            p->print(stdout);
                        }
                    }
                }
            }
        }

        if (net->numProps())
        {
            for (i = 0; i < net->numProps(); i++)
            {
                if (net->propIsString(i))
                    limboPrint(limbo::kNONE, "  + PROPERTY %s %s ", net->propName(i),
                               net->propValue(i));
                if (net->propIsNumber(i))
                    limboPrint(limbo::kNONE, "  + PROPERTY %s %g ", net->propName(i),
                               net->propNumber(i));
                switch (net->propType(i))
                {
                    case 'R': limboPrint(limbo::kNONE, "REAL ");
                        break;
                    case 'I': limboPrint(limbo::kNONE, "INTEGER ");
                        break;
                    case 'S': limboPrint(limbo::kNONE, "STRING ");
                        break;
                    case 'Q': limboPrint(limbo::kNONE, "QUOTESTRING ");
                        break;
                    case 'N': limboPrint(limbo::kNONE, "NUMBER ");
                        break;
                }
                limboPrint(limbo::kNONE, "\n");
            }
        }

        // SHIELD
        count = 0;
        // testing the SHIELD for 5.3, obsolete in 5.4
        if (net->numShields())
        {
            for (i = 0; i < net->numShields(); i++)
            {
                shield = net->shield(i);
                limboPrint(limbo::kNONE, "\n  + SHIELD %s ", shield->shieldName());
                newLayer = 0;
                for (j = 0; j < shield->numPaths(); j++)
                {
                    p = shield->path(j);
                    p->initTraverse();
                    while ((path = (int)p->next()) != DEFIPATH_DONE)
                    {
                        count++;
                        // Don't want the line to be too long
                        if (count >= 5)
                        {
                            limboPrint(limbo::kNONE, "\n");
                            count = 0;
                        }
                        switch (path)
                        {
                            case DEFIPATH_LAYER:
                                if (newLayer == 0)
                                {
                                    limboPrint(limbo::kNONE, "%s ", p->getLayer());
                                    newLayer = 1;
                                } else
                                    limboPrint(limbo::kNONE, "NEW %s ", p->getLayer());
                                break;
                            case DEFIPATH_VIA:
                                limboPrint(limbo::kNONE, "%s ", ignoreViaNames ? "XXX" : p->getVia());
                                break;
                            case DEFIPATH_VIAROTATION:
                                limboPrint(limbo::kNONE, "%s ",
                                           orientStr(p->getViaRotation()));
                                break;
                            case DEFIPATH_WIDTH:
                                limboPrint(limbo::kNONE, "%d ", p->getWidth());
                                break;
                            case DEFIPATH_MASK:
                                limboPrint(limbo::kNONE, "MASK %d ", p->getMask());
                                break;
                            case DEFIPATH_VIAMASK:
                                limboPrint(limbo::kNONE, "MASK %d%d%d ",
                                           p->getViaTopMask(),
                                           p->getViaCutMask(),
                                           p->getViaBottomMask());
                                break;
                            case DEFIPATH_POINT:
                                p->getPoint(&x, &y);
                                limboPrint(limbo::kNONE, "( %d %d ) ", x, y);
                                break;
                            case DEFIPATH_FLUSHPOINT:
                                p->getFlushPoint(&x, &y, &z);
                                limboPrint(limbo::kNONE, "( %d %d %d ) ", x, y, z);
                                break;
                            case DEFIPATH_TAPER:
                                limboPrint(limbo::kNONE, "TAPER ");
                                break;
                            case DEFIPATH_SHAPE:
                                limboPrint(limbo::kNONE, "+ SHAPE %s ", p->getShape());
                                break;
                            case DEFIPATH_STYLE:
                                limboPrint(limbo::kNONE, "+ STYLE %d ", p->getStyle());
                                break;
                        }
                    }
                }
            }
        }

        // layerName width
        if (net->hasWidthRules())
        {
            for (i = 0; i < net->numWidthRules(); i++)
            {
                net->widthRule(i, &layerName, &dist);
                limboPrint(limbo::kNONE, "\n  + WIDTH %s %g ", layerName, dist);
            }
        }

        // layerName spacing
        if (net->hasSpacingRules())
        {
            for (i = 0; i < net->numSpacingRules(); i++)
            {
                net->spacingRule(i, &layerName, &dist, &left, &right);
                if (left == right)
                    limboPrint(limbo::kNONE, "\n  + SPACING %s %g ", layerName, dist);
                else
                    limboPrint(limbo::kNONE, "\n  + SPACING %s %g RANGE %g %g ",
                               layerName, dist, left, right);
            }
        }

        if (net->hasFixedbump())
            limboPrint(limbo::kNONE, "\n  + FIXEDBUMP ");
        if (net->hasFrequency())
            limboPrint(limbo::kNONE, "\n  + FREQUENCY %g ", net->frequency());
        if (net->hasVoltage())
            limboPrint(limbo::kNONE, "\n  + VOLTAGE %g ", net->voltage());
        if (net->hasWeight())
            limboPrint(limbo::kNONE, "\n  + WEIGHT %d ", net->weight());
        if (net->hasCap())
            limboPrint(limbo::kNONE, "\n  + ESTCAP %g ", net->cap());
        if (net->hasSource())
            limboPrint(limbo::kNONE, "\n  + SOURCE %s ", net->source());
        if (net->hasPattern())
            limboPrint(limbo::kNONE, "\n  + PATTERN %s ", net->pattern());
        if (net->hasOriginal())
            limboPrint(limbo::kNONE, "\n  + ORIGINAL %s ", net->original());
        //if (net->hasUse())
        //limboPrint(limbo::kNONE, "\n  + USE %s ", net->use());

        limboPrint(limbo::kNONE, ";\n");
        --numObjs;
        if (numObjs <= 0)
            limboPrint(limbo::kNONE, "END SPECIALNETS\n");

        defDB->add_def_snetpath(defDriver->snetp());
        defDriver->snetp().reset();
        return 0;
    }


    int ndr(defrCallbackType_e c, defiNonDefault* nd, defiUserData ud)
    {
        // For nondefaultrule
        int i;

        checkType(c);
        if (ud != userData) dataError();
        if (c != defrNonDefaultCbkType)
            limboPrint(limbo::kNONE, "BOGUS NONDEFAULTRULE TYPE  ");
        limboPrint(limbo::kNONE, "- %s\n", nd->name());
        if (nd->hasHardspacing())
            limboPrint(limbo::kNONE, "   + HARDSPACING\n");
        for (i = 0; i < nd->numLayers(); i++)
        {
            limboPrint(limbo::kNONE, "   + LAYER %s", nd->layerName(i));
            limboPrint(limbo::kNONE, " WIDTH %d", nd->layerWidthVal(i));
            if (nd->hasLayerDiagWidth(i))
                limboPrint(limbo::kNONE, " DIAGWIDTH %d",
                           nd->layerDiagWidthVal(i));
            if (nd->hasLayerSpacing(i))
                limboPrint(limbo::kNONE, " SPACING %d", nd->layerSpacingVal(i));
            if (nd->hasLayerWireExt(i))
                limboPrint(limbo::kNONE, " WIREEXT %d", nd->layerWireExtVal(i));
            limboPrint(limbo::kNONE, "\n");
        }
        for (i = 0; i < nd->numVias(); i++)
            limboPrint(limbo::kNONE, "   + VIA %s\n", nd->viaName(i));
        for (i = 0; i < nd->numViaRules(); i++)
            limboPrint(limbo::kNONE, "   + VIARULE %s\n", ignoreViaNames ? "XXX" : nd->viaRuleName(i));
        for (i = 0; i < nd->numMinCuts(); i++)
            limboPrint(limbo::kNONE, "   + MINCUTS %s %d\n", nd->cutLayerName(i),
                       nd->numCuts(i));
        for (i = 0; i < nd->numProps(); i++)
        {
            limboPrint(limbo::kNONE, "   + PROPERTY %s %s ", nd->propName(i),
                       nd->propValue(i));
            switch (nd->propType(i))
            {
                case 'R': limboPrint(limbo::kNONE, "REAL\n");
                    break;
                case 'I': limboPrint(limbo::kNONE, "INTEGER\n");
                    break;
                case 'S': limboPrint(limbo::kNONE, "STRING\n");
                    break;
                case 'Q': limboPrint(limbo::kNONE, "QUOTESTRING\n");
                    break;
                case 'N': limboPrint(limbo::kNONE, "NUMBER\n");
                    break;
            }
        }
        --numObjs;
        if (numObjs <= 0)
            limboPrint(limbo::kNONE, "END NONDEFAULTRULES\n");
        return 0;
    }

    int technologyName(defrCallbackType_e c, const char* string, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        limboPrint(limbo::kNONE, "TECHNOLOGY %s ;\n", string);
        return 0;
    }

    int designName(defrCallbackType_e c, const char* str, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDB->set_def_design(str);

        return 0;
    }


    int cs(defrCallbackType_e c, int num, defiUserData ud)
    {
        checkType(c);

        if (ud != userData) dataError();

        switch (c)
        {
            case defrComponentStartCbkType :
                // name = "COMPONENTS";
                defDB->resize_def_component(num);
                break;
            case defrNetStartCbkType :
                // name = "NETS";
                defDB->resize_def_net(num);
                break;
            case defrStartPinsCbkType :
                // name = "PINS";
                defDB->resize_def_pin(num);
                break;
            case defrViaStartCbkType :
                // name = "VIAS";
                break;
            case defrRegionStartCbkType :
                // name = "REGIONS";
                break;
            case defrSNetStartCbkType :
                // name = "SPECIALNETS";
                break;
            case defrGroupsStartCbkType :
                // name = "GROUPS";
                break;
            case defrScanchainsStartCbkType :
                // name = "SCANCHAINS";
                break;
            case defrIOTimingsStartCbkType :
                // name = "IOTIMINGS";
                break;
            case defrFPCStartCbkType :
                // name = "FLOORPLANCONSTRAINTS";
                break;
            case defrTimingDisablesStartCbkType :
                // name = "TIMING DISABLES";
                break;
            case defrPartitionsStartCbkType :
                // name = "PARTITIONS";
                break;
            case defrPinPropStartCbkType :
                // name = "PINPROPERTIES";
                break;
            case defrBlockageStartCbkType :
                // name = "BLOCKAGES";
                defDB->resize_def_blockage(num);
                break;
            case defrSlotStartCbkType :
                // name = "SLOTS";
                break;
            case defrFillStartCbkType :
                // name = "FILLS";
                break;
            case defrNonDefaultStartCbkType :
                // name = "NONDEFAULTRULES";
                break;
            case defrStylesStartCbkType :
                // name = "STYLES";
                break;
            default :
                // name = "BOGUS";
                return 1;
        }
        return 0;
    }

    int constraintst(defrCallbackType_e c, int num, defiUserData ud)
    {
        // Handles both constraints and assertions
        checkType(c);
        if (ud != userData) dataError();
        if (c == defrConstraintsStartCbkType)
            limboPrint(limbo::kNONE, "\nCONSTRAINTS %d ;\n\n", num);
        else
            limboPrint(limbo::kNONE, "\nASSERTIONS %d ;\n\n", num);
        numObjs = num;
        return 0;
    }

    void operand(defrCallbackType_e c, defiAssertion* a, int ind)
    {
        int i, first = 1;
        char* netName;
        char* fromInst, * fromPin, * toInst, * toPin;

        if (a->isSum())
        {
            // Sum in operand, recursively call operand
            limboPrint(limbo::kNONE, "- SUM ( ");
            a->unsetSum();
            isSumSet = 1;
            begOperand = 0;
            operand (c, a, ind);
            limboPrint(limbo::kNONE, ") ");
        } else
        {
            // operand
            if (ind >= a->numItems())
            {
                limboPrint(limbo::kNONE, "ERROR: when writing out SUM in Constraints.\n");
                return;
            }
            if (begOperand)
            {
                limboPrint(limbo::kNONE, "- ");
                begOperand = 0;
            }
            for (i = ind; i < a->numItems(); i++)
            {
                if (a->isNet(i))
                {
                    a->net(i, &netName);
                    if (!first)
                        limboPrint(limbo::kNONE, ", "); // print , as separator
                    limboPrint(limbo::kNONE, "NET %s ", netName);
                } else if (a->isPath(i))
                {
                    a->path(i, &fromInst, &fromPin, &toInst,
                            &toPin);
                    if (!first)
                        limboPrint(limbo::kNONE, ", ");
                    limboPrint(limbo::kNONE, "PATH %s %s %s %s ", fromInst, fromPin, toInst,
                               toPin);
                } else if (isSumSet)
                {
                    // SUM within SUM, reset the flag
                    a->setSum();
                    operand(c, a, i);
                }
                first = 0;
            }

        }
    }

    int constraint(defrCallbackType_e c, defiAssertion* a, defiUserData ud)
    {
        // Handles both constraints and assertions

        checkType(c);
        if (ud != userData) dataError();
        if (a->isWiredlogic())
            // Wirelogic
            limboPrint(limbo::kNONE, "- WIREDLOGIC %s + MAXDIST %g ;\n",
                    // Wiredlogic dist is also store in fallMax
                    //              a->netName(), a->distance());
                       a->netName(), a->fallMax());
        else
        {
            // Call the operand function
            isSumSet = 0;    // reset the global variable
            begOperand = 1;
            operand (c, a, 0);
            // Get the Rise and Fall
            if (a->hasRiseMax())
                limboPrint(limbo::kNONE, "+ RISEMAX %g ", a->riseMax());
            if (a->hasFallMax())
                limboPrint(limbo::kNONE, "+ FALLMAX %g ", a->fallMax());
            if (a->hasRiseMin())
                limboPrint(limbo::kNONE, "+ RISEMIN %g ", a->riseMin());
            if (a->hasFallMin())
                limboPrint(limbo::kNONE, "+ FALLMIN %g ", a->fallMin());
            limboPrint(limbo::kNONE, ";\n");
        }
        --numObjs;
        if (numObjs <= 0)
        {
            if (c == defrConstraintCbkType)
                limboPrint(limbo::kNONE, "END CONSTRAINTS\n");
            else
                limboPrint(limbo::kNONE, "END ASSERTIONS\n");
        }
        return 0;
    }


    int propstart(defrCallbackType_e c, void*, defiUserData)
    {
        checkType(c);
        limboPrint(limbo::kNONE, "\nPROPERTYDEFINITIONS\n");
        isProp = 1;

        return 0;
    }


    int prop(defrCallbackType_e c, defiProp* p, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        if (strcmp(p->propType(), "design") == 0)
            limboPrint(limbo::kNONE, "DESIGN %s ", p->propName());
        else if (strcmp(p->propType(), "net") == 0)
            limboPrint(limbo::kNONE, "NET %s ", p->propName());
        else if (strcmp(p->propType(), "component") == 0)
            limboPrint(limbo::kNONE, "COMPONENT %s ", p->propName());
        else if (strcmp(p->propType(), "specialnet") == 0)
            limboPrint(limbo::kNONE, "SPECIALNET %s ", p->propName());
        else if (strcmp(p->propType(), "group") == 0)
            limboPrint(limbo::kNONE, "GROUP %s ", p->propName());
        else if (strcmp(p->propType(), "row") == 0)
            limboPrint(limbo::kNONE, "ROW %s ", p->propName());
        else if (strcmp(p->propType(), "componentpin") == 0)
            limboPrint(limbo::kNONE, "COMPONENTPIN %s ", p->propName());
        else if (strcmp(p->propType(), "region") == 0)
            limboPrint(limbo::kNONE, "REGION %s ", p->propName());
        else if (strcmp(p->propType(), "nondefaultrule") == 0)
            limboPrint(limbo::kNONE, "NONDEFAULTRULE %s ", p->propName());
        if (p->dataType() == 'I')
            limboPrint(limbo::kNONE, "INTEGER ");
        if (p->dataType() == 'R')
            limboPrint(limbo::kNONE, "REAL ");
        if (p->dataType() == 'S')
            limboPrint(limbo::kNONE, "STRING ");
        if (p->dataType() == 'Q')
            limboPrint(limbo::kNONE, "STRING ");
        if (p->hasRange())
        {
            limboPrint(limbo::kNONE, "RANGE %g %g ", p->left(),
                       p->right());
        }
        if (p->hasNumber())
            limboPrint(limbo::kNONE, "%g ", p->number());
        if (p->hasString())
            limboPrint(limbo::kNONE, "\"%s\" ", p->string());
        limboPrint(limbo::kNONE, ";\n");

        return 0;
    }


    int propend(defrCallbackType_e c, void*, defiUserData)
    {
        checkType(c);
        if (isProp)
        {
            limboPrint(limbo::kNONE, "END PROPERTYDEFINITIONS\n\n");
            isProp = 0;
        }

        return 0;
    }


    int hist(defrCallbackType_e c, const char* h, defiUserData ud)
    {
        checkType(c);
        defrSetCaseSensitivity(0);
        if (ud != userData) dataError();
        limboPrint(limbo::kNONE, "HISTORY %s ;\n", h);
        defrSetCaseSensitivity(1);
        return 0;
    }


    int an(defrCallbackType_e c, const char* h, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        limboPrint(limbo::kNONE, "ARRAY %s ;\n", h);
        return 0;
    }


    int fn(defrCallbackType_e c, const char* h, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        limboPrint(limbo::kNONE, "FLOORPLAN %s ;\n", h);
        return 0;
    }


    int busbitChars(defrCallbackType_e c, const char* h, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDB->set_def_busbitchars(h);
        return 0;
    }


    int vers(defrCallbackType_e c, double d, defiUserData ud)
    {
        checkType(c);
        if (ud != userData)
            dataError();
        limboPrint(limbo::kNONE, "VERSION %g ;\n", d);
        curVer = d;

        limboPrint(limbo::kNONE, "ALIAS alias1 aliasValue1 1 ;\n");
        limboPrint(limbo::kNONE, "ALIAS alias2 aliasValue2 0 ;\n");

        return 0;
    }


    int versStr(defrCallbackType_e c, const char* versionName, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDB->set_def_version(versionName);
        return 0;
    }


    int units(defrCallbackType_e c, double d, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDB->set_def_unit(d);
        return 0;
    }


    int casesens(defrCallbackType_e c, int d, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        if (d == 1)
            limboPrint(limbo::kNONE, "NAMESCASESENSITIVE ON ;\n", d);
        else
            limboPrint(limbo::kNONE, "NAMESCASESENSITIVE OFF ;\n", d);
        return 0;
    }


    int cls(defrCallbackType_e c, void* cl, defiUserData ud)
    {
        defiSite* site;  // Site and Canplace and CannotOccupy
        defiBox* box;  // DieArea and
        defiPinCap* pc;
        defiPin* pin;
        int i; 
        //int j;
        defiRow* row;
        //defiTrack* track;
        defiGcellGrid* gcg;
        defiVia* via;
        defiRegion* re;
        defiGroup* group;
        defiComponentMaskShiftLayer* maskShiftLayer = NULL;
        //defiScanchain* sc;
        //defiIOTiming* iot;
        //defiFPC* fpc;
        //defiTimingDisable* td;
        defiPartition* part;
        defiPinProp* pprop;
        defiBlockage* block;
        //defiSlot* slots;
        //defiFill* fills;
        //defiStyles* styles;
        int xl, yl, xh, yh;
        char *name;
        //char *a1, *b1;
        //char **inst, **inPin, **outPin;
        //int  *bits;
        //int  size;
        //int corner, typ;
        const char *itemT;
        char dir;
        //defiPinAntennaModel* aModel;
        //struct defiPoints points;

        checkType(c);
        if (ud != userData) dataError();
        switch (c)
        {

            case defrSiteCbkType :
                site = (defiSite*)cl;
                limboPrint(limbo::kNONE, "SITE %s %g %g %s ", site->name(),
                           site->x_orig(), site->y_orig(),
                           orientStr(site->orient()));
                limboPrint(limbo::kNONE, "DO %g BY %g STEP %g %g ;\n",
                           site->x_num(), site->y_num(),
                           site->x_step(), site->y_step());
                break;
            case defrCanplaceCbkType :
                site = (defiSite*)cl;
                limboPrint(limbo::kNONE, "CANPLACE %s %g %g %s ", site->name(),
                           site->x_orig(), site->y_orig(),
                           orientStr(site->orient()));
                limboPrint(limbo::kNONE, "DO %g BY %g STEP %g %g ;\n",
                           site->x_num(), site->y_num(),
                           site->x_step(), site->y_step());
                break;
            case defrCannotOccupyCbkType :
                site = (defiSite*)cl;
                limboPrint(limbo::kNONE, "CANNOTOCCUPY %s %g %g %s ",
                           site->name(), site->x_orig(),
                           site->y_orig(), orientStr(site->orient()));
                limboPrint(limbo::kNONE, "DO %g BY %g STEP %g %g ;\n",
                           site->x_num(), site->y_num(),
                           site->x_step(), site->y_step());
                break;
            case defrDieAreaCbkType :
                box = (defiBox*)cl;
                defDB->set_def_diearea(box->xl(), box->yl(), box->xh(), box->yh());
                break;
            case defrPinCapCbkType :
                pc = (defiPinCap*)cl;
                if (testDebugPrint)
                {
                    pc->print(stdout);
                } else
                {
                    limboPrint(limbo::kNONE, "MINPINS %d WIRECAP %g ;\n",
                               pc->pin(), pc->cap());
                    --numObjs;
                    if (numObjs <= 0)
                        limboPrint(limbo::kNONE, "END DEFAULTCAP\n");
                }
                break;
            case defrPinCbkType :
                pin = (defiPin*)cl;
                defDriver->pin().pin_name = pin->pinName();
                defDriver->pin().net_name = pin->netName();
                if (pin->hasDirection())
                    defDriver->pin().direct = pin->direction();
                if (pin->hasUse())
                    defDriver->pin().use = pin->use();
                if (pin->hasPlacement())
                {
                    if (pin->isPlaced())
                    {
                        defDriver->pin().status = "PLACED";
                        defDriver->pin().orient = orientStr(pin->orient());
                        defDriver->pin().origin[0] = pin->placementX();
                        defDriver->pin().origin[1] = pin->placementY();
                    }
                    if (pin->isFixed())
                    {
                        defDriver->pin().status = "FIXED";
                    }
                    if (pin->isUnplaced())
                    {
                        defDriver->pin().status = "UNPLACED";
                    }
                    defDriver->pin().orient = orientStr(pin->orient());
                    defDriver->pin().origin[0] = pin->placementX();
                    defDriver->pin().origin[1] = pin->placementY();
                }
                if (pin->hasLayer())
                {
                    defDriver->pin().vLayer.resize(pin->numLayer());
                    defDriver->pin().vBbox.resize(pin->numLayer(), std::vector<int32_t>(4));
                    for (int i = 0; i < pin->numLayer(); i++)
                    {
                        defDriver->pin().vLayer[i] = pin->layer(i);
                        pin->bounds(i,
                                    &(defDriver->pin().vBbox[i][0]),
                                    &(defDriver->pin().vBbox[i][1]),
                                    &(defDriver->pin().vBbox[i][2]),
                                    &(defDriver->pin().vBbox[i][3])
                        );
                    }
                }
                defDB->add_def_pin(defDriver->pin());
                defDriver->pin().reset();
                break;
            case defrRowCbkType :
                row = (defiRow*)cl;
                defDriver->row().row_name = row->name();
                defDriver->row().macro_name = row->macro();
                defDriver->row().origin[0] = row->x();
                defDriver->row().origin[1] = row->y();
                defDriver->row().orient = orientStr(row->orient());
                if (row->hasDo())
                {
                    defDriver->row().repeat[0] = row->xNum();
                    defDriver->row().repeat[1] = row->yNum();
                    defDriver->row().step[0] = row->xStep();
                    defDriver->row().step[1] = row->yStep();
                }
                defDB->add_def_row(defDriver->row());
                defDriver->row().reset();
                break;
            case defrTrackCbkType :
                /*
                    track = (defiTrack*)cl;
                    if (track->firstTrackMask())
                    {
                        if (track->sameMask())
                        {
                            //limboPrint(limbo::kNONE, "TRACKS %s %g DO %g STEP %g MASK %d SAMEMASK LAYER ",
                              //      track->macro(), track->x(),
                                //    track->xNum(), track->xStep(),
                                  //  track->firstTrackMask());
                                  defDriver->track().macro_name = track->macro();
                                  defDriver->track().x = track->x();
                                  defDriver->track().xNum = track->xNum();
                                  defDriver->track().xStep = track->xStep();

                        } else
                        {
                            //limboPrint(limbo::kNONE, "TRACKS %s %g DO %g STEP %g MASK %d LAYER ",
                              //      track->macro(), track->x(),
                                //    track->xNum(), track->xStep(),
                                  //  track->firstTrackMask());
                                  defDriver->track().macro_name = track->macro();
                                  defDriver->track().x = track->x();
                                  defDriver->track().xNum = track->xNum();
                                  defDriver->track().xStep = track->xStep();
                        }
                    } else
                    {
                        //limboPrint(limbo::kNONE, "TRACKS %s %g DO %g STEP %g LAYER ",
                          //      track->macro(), track->x(),
                            //    track->xNum(), track->xStep());
                            defDriver->track().macro_name = track->macro();
                                  defDriver->track().x = track->x();
                                  defDriver->track().xNum = track->xNum();
                                  defDriver->track().xStep = track->xStep();
                    }
                    for (i = 0; i < track->numLayers(); i++)
                        //limboPrint(limbo::kNONE, "%s ", track->layer(i));
                    //limboPrint(limbo::kNONE, ";\n");
                    defDriver->track().layer.push_back(track->layer(i));
                    defDB->add_def_track(defDriver->track());
                    defDriver->track().reset();
                    */
                break;
            case defrGcellGridCbkType :
                gcg = (defiGcellGrid*)cl;
                limboPrint(limbo::kNONE, "GCELLGRID %s %d DO %d STEP %g ;\n",
                           gcg->macro(), gcg->x(),
                           gcg->xNum(), gcg->xStep());
                break;
            case defrViaCbkType :
                via = (defiVia *) cl;
                defDriver->via().name = via->name();
                for (i = 0; i < via->numLayers(); i++) {
                    via->layer(i, &name, &xl, &yl, &xh, &yh);

                    defDriver->via().layers.emplace_back(name);
                    defDriver->via().xl.emplace_back(xl);
                    defDriver->via().yl.emplace_back(yl);
                    defDriver->via().xh.emplace_back(xh);
                    defDriver->via().yh.emplace_back(yh);
                }
                --numObjs;
                if (numObjs <= 0) {
                    defDB->add_def_via(defDriver->via());
                    defDriver->via().reset();
                }
                break;
            case defrRegionCbkType :
            {

                re = (defiRegion*)cl;
                //limboPrint(limbo::kNONE, "defrRegionCbkType - %s \n", re->name());
                defDriver->region().name = re->name();
                //int numrect = re->numRectangles();
                //cout<< "numrect" << numrect<<endl;
                for (i = 0; i < re->numRectangles(); i++) {
                    // limboPrint(limbo::kNONE, "%d %d %d %d \n", re->xl(i),
                    //     re->yl(i), re->xh(i),
                    //   re->yh(i));
                    //cout<<"i: "<< i<<endl;
                    //int xlt = re->xl(i);
                    //int ylt = re->yl(i);
                    //int xht = re->xh(i);
                    //int yht = re->yh(i);
                    // cout<<"finished"<<endl;
                    defDriver->region().xl.push_back(re->xl(i));
                    defDriver->region().xh.push_back(re->xh(i));
                    defDriver->region().yl.push_back(re->yl(i));
                    defDriver->region().yh.push_back(re->yh(i));
                }
                if (re->hasType()) {
                    // limboPrint(limbo::kNONE, "+ TYPE %s\n", re->type());
                    defDriver->region().type = re->type();
                }
                if (re->numProps())
                {
                    for (i = 0; i < re->numProps(); i++)
                    {
                        limboPrint(limbo::kNONE, "+ PROPERTY %s %s ", re->propName(i),
                                   re->propValue(i));
                        switch (re->propType(i))
                        {
                            case 'R': limboPrint(limbo::kNONE, "REAL ");
                                break;
                            case 'I': limboPrint(limbo::kNONE, "INTEGER ");
                                break;
                            case 'S': limboPrint(limbo::kNONE, "STRING ");
                                break;
                            case 'Q': limboPrint(limbo::kNONE, "QUOTESTRING ");
                                break;
                            case 'N': limboPrint(limbo::kNONE, "NUMBER ");
                                break;
                        }
                    }
                }
                //limboPrint(limbo::kNONE, ";\n");
                --numObjs;
                if (numObjs <= 0)
                {
                    //limboPrint(limbo::kNONE, "END REGIONS\n");
                    defDB->add_def_region(defDriver->region());
                    defDriver->region().reset();
                }
            }

                break;
            case defrGroupNameCbkType :
                if ((char*)cl)
                {
                    //limboPrint(limbo::kNONE, "defrGroupNameCbkType - %s \n", (char*)cl);
                    defDriver->group().name = (char*) cl;
                }
                break;
            case defrGroupMemberCbkType :
                if ((char*)cl)
                {
                    //limboPrint(limbo::kNONE, "defrGroupMemberCbkType %s \n", (char*)cl);
                    defDriver->group().group_members.push_back((char*) cl);
                }
                break;
            case defrComponentMaskShiftLayerCbkType :
                limboPrint(limbo::kNONE, "COMPONENTMASKSHIFT ");

                for (i = 0; i < maskShiftLayer->numMaskShiftLayers(); i++)
                {
                    limboPrint(limbo::kNONE, "%s ", maskShiftLayer->maskShiftLayer(i));
                }
                limboPrint(limbo::kNONE, ";\n");
                break;
            case defrGroupCbkType :

                group = (defiGroup*)cl;
                //limboPrint(limbo::kNONE, "defrGroupCbkType \n ");
                if (group->hasMaxX() | group->hasMaxY()
                    | group->hasPerim())
                {
                    limboPrint(limbo::kNONE, "\n  + SOFT ");
                    if (group->hasPerim())
                        limboPrint(limbo::kNONE, "MAXHALFPERIMETER %d ",
                                   group->perim());
                    if (group->hasMaxX())
                        limboPrint(limbo::kNONE, "MAXX %d ", group->maxX());
                    if (group->hasMaxY())
                        limboPrint(limbo::kNONE, "MAXY %d ", group->maxY());
                }
                if (group->hasRegionName()) {
                    //if (0)
                    //limboPrint(limbo::kNONE, "\n  + REGION %s ", group->regionName());
                    defDriver->group().regions.push_back(group->regionName());
                }
                if (group->hasRegionBox())
                {
                    int *gxl, *gyl, *gxh, *gyh;
                    int size;
                    group->regionRects(&size, &gxl, &gyl, &gxh, &gyh);
                    for (i = 0; i < size; i++)
                        limboPrint(limbo::kNONE, "REGION %d %d %d %d ", gxl[i], gyl[i],
                                   gxh[i], gyh[i]);
                }
                if (group->numProps())
                {
                    for (i = 0; i < group->numProps(); i++)
                    {
                        limboPrint(limbo::kNONE, "\n  + PROPERTY %s %s ",
                                   group->propName(i),
                                   group->propValue(i));
                        switch (group->propType(i))
                        {
                            case 'R': limboPrint(limbo::kNONE, "REAL ");
                                break;
                            case 'I': limboPrint(limbo::kNONE, "INTEGER ");
                                break;
                            case 'S': limboPrint(limbo::kNONE, "STRING ");
                                break;
                            case 'Q': limboPrint(limbo::kNONE, "QUOTESTRING ");
                                break;
                            case 'N': limboPrint(limbo::kNONE, "NUMBER ");
                                break;
                        }
                    }
                }
                //limboPrint(limbo::kNONE, " ;\n");
                --numObjs;
                if (numObjs <= 0) {
                    //limboPrint(limbo::kNONE, "END GROUPS\n");
                    defDB->add_def_group(defDriver->group());
                    defDriver->group().reset();
                }
                break;
            case defrPartitionCbkType :
                part = (defiPartition*)cl;
                limboPrint(limbo::kNONE, "- %s ", part->name());
                if (part->isSetupRise() |
                    part->isSetupFall() |
                    part->isHoldRise() |
                    part->isHoldFall())
                {
                    // has turnoff
                    limboPrint(limbo::kNONE, "TURNOFF ");
                    if (part->isSetupRise())
                        limboPrint(limbo::kNONE, "SETUPRISE ");
                    if (part->isSetupFall())
                        limboPrint(limbo::kNONE, "SETUPFALL ");
                    if (part->isHoldRise())
                        limboPrint(limbo::kNONE, "HOLDRISE ");
                    if (part->isHoldFall())
                        limboPrint(limbo::kNONE, "HOLDFALL ");
                }
                itemT = part->itemType();
                dir = part->direction();
                if (strcmp(itemT, "CLOCK") == 0)
                {
                    if (dir == 'T')    // toclockpin
                        limboPrint(limbo::kNONE, "+ TOCLOCKPIN %s %s ",
                                   part->instName(),
                                   part->pinName());
                    if (dir == 'F')    // fromclockpin
                        limboPrint(limbo::kNONE, "+ FROMCLOCKPIN %s %s ",
                                   part->instName(),
                                   part->pinName());
                    if (part->hasMin())
                        limboPrint(limbo::kNONE, "MIN %g %g ",
                                   part->partitionMin(),
                                   part->partitionMax());
                    if (part->hasMax())
                        limboPrint(limbo::kNONE, "MAX %g %g ",
                                   part->partitionMin(),
                                   part->partitionMax());
                    limboPrint(limbo::kNONE, "PINS ");
                    for (i = 0; i < part->numPins(); i++)
                        limboPrint(limbo::kNONE, "%s ", part->pin(i));
                } else if (strcmp(itemT, "IO") == 0)
                {
                    if (dir == 'T')    // toiopin
                        limboPrint(limbo::kNONE, "+ TOIOPIN %s %s ",
                                   part->instName(),
                                   part->pinName());
                    if (dir == 'F')    // fromiopin
                        limboPrint(limbo::kNONE, "+ FROMIOPIN %s %s ",
                                   part->instName(),
                                   part->pinName());
                } else if (strcmp(itemT, "COMP") == 0)
                {
                    if (dir == 'T')    // tocomppin
                        limboPrint(limbo::kNONE, "+ TOCOMPPIN %s %s ",
                                   part->instName(),
                                   part->pinName());
                    if (dir == 'F')    // fromcomppin
                        limboPrint(limbo::kNONE, "+ FROMCOMPPIN %s %s ",
                                   part->instName(),
                                   part->pinName());
                }
                limboPrint(limbo::kNONE, ";\n");
                --numObjs;
                if (numObjs <= 0)
                    limboPrint(limbo::kNONE, "END PARTITIONS\n");
                break;
            case defrPinPropCbkType :
                pprop = (defiPinProp*)cl;
                if (pprop->isPin())
                    limboPrint(limbo::kNONE, "- PIN %s ", pprop->pinName());
                else
                    limboPrint(limbo::kNONE, "- %s %s ",
                               pprop->instName(),
                               pprop->pinName());
                limboPrint(limbo::kNONE, ";\n");
                if (pprop->numProps() > 0)
                {
                    for (i = 0; i < pprop->numProps(); i++)
                    {
                        limboPrint(limbo::kNONE, "  + PROPERTY %s %s ",
                                   pprop->propName(i),
                                   pprop->propValue(i));
                        switch (pprop->propType(i))
                        {
                            case 'R': limboPrint(limbo::kNONE, "REAL ");
                                break;
                            case 'I': limboPrint(limbo::kNONE, "INTEGER ");
                                break;
                            case 'S': limboPrint(limbo::kNONE, "STRING ");
                                break;
                            case 'Q': limboPrint(limbo::kNONE, "QUOTESTRING ");
                                break;
                            case 'N': limboPrint(limbo::kNONE, "NUMBER ");
                                break;
                        }
                    }
                    limboPrint(limbo::kNONE, ";\n");
                }
                --numObjs;
                if (numObjs <= 0)
                    limboPrint(limbo::kNONE, "END PINPROPERTIES\n");
                break;
            case defrBlockageCbkType :
                block = (defiBlockage*)cl;
                if (block->hasPlacement())
                {
                    std::vector<std::vector<int> > vBbox (block->numRectangles(), std::vector<int>(4));
                    for (i = 0; i < block->numRectangles(); i++)
                    {
                        vBbox[i][0] = block->xl(i);
                        vBbox[i][1] = block->yl(i);
                        vBbox[i][2] = block->xh(i);
                        vBbox[i][3] = block->yh(i);
                    }
                    defDB->add_def_placement_blockage(vBbox);
                }
                break;

            default: return 0;
        }
        return 0;
    }

    int dividerChar(defrCallbackType_e c, const char* h, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        defDB->set_def_dividerchar(h);
        return 0;
    }


    int ext(defrCallbackType_e t, const char* c, defiUserData ud)
    {
        const char* name;

        checkType(t);
        if (ud != userData) dataError();

        switch (t)
        {
            case defrNetExtCbkType : name = "net"; break;
            case defrComponentExtCbkType : name = "component"; break;
            case defrPinExtCbkType : name = "pin"; break;
            case defrViaExtCbkType : name = "via"; break;
            case defrNetConnectionExtCbkType : name = "net connection"; break;
            case defrGroupExtCbkType : name = "group"; break;
            case defrScanChainExtCbkType : name = "scanchain"; break;
            case defrIoTimingsExtCbkType : name = "io timing"; break;
            case defrPartitionsExtCbkType : name = "partition"; break;
            default: name = "BOGUS"; return 1;
        }
        limboPrint(limbo::kNONE, "  %s extension %s\n", name, c);
        return 0;
    }

    int extension(defrCallbackType_e c, const char* extsn, defiUserData ud)
    {
        checkType(c);
        if (ud != userData) dataError();
        limboPrint(limbo::kNONE, "BEGINEXT %s\n", extsn);
        return 0;
    }

    void* mallocCB(size_t size)
    {
        return malloc(size);
    }

    void* reallocCB(void* name, size_t size)
    {
        return realloc(name, size);
    }

    void freeCB(void* name)
    {
        free(name);
        return;
    }


    BEGIN_LEFDEF_PARSER_NAMESPACE
    extern long long nlines;
    END_LEFDEF_PARSER_NAMESPACE
    static int ccr1131444 = 0;

    void lineNumberCB(long long lineNo)
    {

        // The CCR 1131444 tests ability of the DEF parser to count
        // input line numbers out of 32-bit int range. On the first callback
        // call 10G lines will be added to line counter. It should be
        // reflected in output.
        if (ccr1131444)
        {
            lineNo += 10000000000LL;
            defrSetNLines(lineNo);
            ccr1131444 = 0;
        }

#ifdef _WIN32
        limboPrint(limbo::kNONE, "Parsed %I64d number of lines!!\n", lineNo);
#else
        limboPrint(limbo::kNONE, "Parsed %lld number of lines!!\n", lineNo);
#endif
    }

    int unUsedCB(defrCallbackType_e, void*, defiUserData)
    {
        limboPrint(limbo::kNONE, "This callback is not used.\n");
        return 0;
    }

    void printWarning(const char *str)
    {
        limboPrint(limbo::kWARN, "%s\n", str);
    }
///@} =========== end of callbacks ============

    bool Driver::parse_file(const std::string &filename)
    {
        isProp = 0;
        setSNetWireCbk = 0;
        isSessionless = 0;
        ignoreRowNames = 0;
        ignoreViaNames = 0;
        testDebugPrint = 0;
        int noCalls = 0;
        //  long start_mem;
        int retStr = 1;
        int noNetCb = 0;
        int line_num_print_interval = 50;

        if (isSessionless)
        {
            defrSetLongLineNumberFunction(lineNumberCB);
            defrSetDeltaNumberLines(line_num_print_interval);
        }

        defrInitSession(isSessionless ? 0 : 1);

        if (noCalls == 0)
        {

            defrSetWarningLogFunction(printWarning);


            defrSetUserData((void*)3);
            defrSetDesignCbk(designName);
            defrSetTechnologyCbk(technologyName);
            defrSetExtensionCbk(extension);
            defrSetDesignEndCbk(done);
            //defrSetPropDefStartCbk(propstart);
            //defrSetPropCbk(prop);
            //defrSetPropDefEndCbk(propend);
            /* Test for CCR 766289*/
            if (!noNetCb)
                defrSetNetCbk(netf);
            defrSetNetNameCbk(netNamef);
            defrSetNetNonDefaultRuleCbk(nondefRulef);
            defrSetNetSubnetNameCbk(subnetNamef);
            defrSetNetPartialPathCbk(netpath);
            defrSetSNetCbk(snetf);
            defrSetSNetPartialPathCbk(snetpath);
            if (setSNetWireCbk)
                defrSetSNetWireCbk(snetwire);
            defrSetComponentMaskShiftLayerCbk(compMSL);
            defrSetComponentCbk(compf);
            defrSetAddPathToNet();
            defrSetHistoryCbk(hist);
            defrSetConstraintCbk(constraint);
            defrSetAssertionCbk(constraint);
            defrSetArrayNameCbk(an);
            defrSetFloorPlanNameCbk(fn);
            defrSetDividerCbk(dividerChar);
            defrSetBusBitCbk(busbitChars);
            //defrSetNonDefaultCbk(ndr);

            defrSetAssertionsStartCbk(constraintst);
            defrSetConstraintsStartCbk(constraintst);
            defrSetComponentStartCbk(cs);
            defrSetPinPropStartCbk(cs);
            defrSetNetStartCbk(cs);
            defrSetStartPinsCbk(cs);
            defrSetViaStartCbk(cs);
            defrSetRegionStartCbk(cs);
            defrSetSNetStartCbk(cs);
            defrSetGroupsStartCbk(cs);
            defrSetScanchainsStartCbk(cs);
            defrSetIOTimingsStartCbk(cs);
            defrSetFPCStartCbk(cs);
            defrSetTimingDisablesStartCbk(cs);
            defrSetPartitionsStartCbk(cs);
            defrSetBlockageStartCbk(cs);
            defrSetSlotStartCbk(cs);
            defrSetFillStartCbk(cs);
            defrSetNonDefaultStartCbk(cs);
            defrSetStylesStartCbk(cs);

            // All of the extensions point to the same function.
            defrSetNetExtCbk(ext);
            defrSetComponentExtCbk(ext);
            defrSetPinExtCbk(ext);
            defrSetViaExtCbk(ext);
            defrSetNetConnectionExtCbk(ext);
            defrSetGroupExtCbk(ext);
            defrSetScanChainExtCbk(ext);
            defrSetIoTimingsExtCbk(ext);
            defrSetPartitionsExtCbk(ext);

            defrSetUnitsCbk(units);
            if (!retStr)
                defrSetVersionCbk(vers);
            else
                defrSetVersionStrCbk(versStr);
            defrSetCaseSensitiveCbk(casesens);

            // The following calls are an example of using one function "cls"
            // to be the callback for many DIFFERENT types of constructs.
            // We have to cast the function type to meet the requirements
            // of each different set function.
            defrSetSiteCbk((defrSiteCbkFnType)cls);
            defrSetCanplaceCbk((defrSiteCbkFnType)cls);
            defrSetCannotOccupyCbk((defrSiteCbkFnType)cls);
            defrSetDieAreaCbk((defrBoxCbkFnType)cls);
            defrSetPinCapCbk((defrPinCapCbkFnType)cls);
            defrSetPinCbk((defrPinCbkFnType)cls);
            defrSetPinPropCbk((defrPinPropCbkFnType)cls);
            defrSetDefaultCapCbk((defrIntegerCbkFnType)cls);
            defrSetRowCbk((defrRowCbkFnType)cls);
            //defrSetTrackCbk((defrTrackCbkFnType)cls);
            //defrSetGcellGridCbk((defrGcellGridCbkFnType)cls);
            defrSetViaCbk((defrViaCbkFnType)cls);
            defrSetRegionCbk((defrRegionCbkFnType)cls);
            defrSetGroupNameCbk((defrStringCbkFnType)cls);
            defrSetGroupMemberCbk((defrStringCbkFnType)cls);
            defrSetGroupCbk((defrGroupCbkFnType)cls);
            defrSetScanchainCbk((defrScanchainCbkFnType)cls);
            defrSetIOTimingCbk((defrIOTimingCbkFnType)cls);
            defrSetFPCCbk((defrFPCCbkFnType)cls);
            defrSetTimingDisableCbk((defrTimingDisableCbkFnType)cls);
            defrSetPartitionCbk((defrPartitionCbkFnType)cls);
            defrSetBlockageCbk((defrBlockageCbkFnType)cls);
            defrSetSlotCbk((defrSlotCbkFnType)cls);
            defrSetFillCbk((defrFillCbkFnType)cls);
            defrSetStylesCbk((defrStylesCbkFnType)cls);

            defrSetAssertionsEndCbk(endfunc);
            defrSetComponentEndCbk(endfunc);
            defrSetConstraintsEndCbk(endfunc);
            defrSetNetEndCbk(endfunc);
            defrSetFPCEndCbk(endfunc);
            defrSetFPCEndCbk(endfunc);
            defrSetGroupsEndCbk(endfunc);
            defrSetIOTimingsEndCbk(endfunc);
            defrSetNetEndCbk(endfunc);
            defrSetPartitionsEndCbk(endfunc);
            defrSetRegionEndCbk(endfunc);
            defrSetSNetEndCbk(endfunc);
            defrSetScanchainsEndCbk(endfunc);
            defrSetPinEndCbk(endfunc);
            defrSetTimingDisablesEndCbk(endfunc);
            defrSetViaEndCbk(endfunc);
            defrSetPinPropEndCbk(endfunc);
            defrSetBlockageEndCbk(endfunc);
            defrSetSlotEndCbk(endfunc);
            defrSetFillEndCbk(endfunc);
            defrSetNonDefaultEndCbk(endfunc);
            defrSetStylesEndCbk(endfunc);

            defrSetMallocFunction(mallocCB);
            defrSetReallocFunction(reallocCB);
            defrSetFreeFunction(freeCB);

            //defrSetRegisterUnusedCallbacks();

            // Testing to set the number of warnings
            defrSetAssertionWarnings(3);
            defrSetBlockageWarnings(3);
            defrSetCaseSensitiveWarnings(3);
            defrSetComponentWarnings(3);
            defrSetConstraintWarnings(0);
            defrSetDefaultCapWarnings(3);
            defrSetGcellGridWarnings(3);
            defrSetIOTimingWarnings(3);
            defrSetNetWarnings(3);
            defrSetNonDefaultWarnings(3);
            defrSetPinExtWarnings(3);
            defrSetPinWarnings(3);
            defrSetRegionWarnings(3);
            defrSetRowWarnings(3);
            defrSetScanchainWarnings(3);
            defrSetSNetWarnings(3);
            defrSetStylesWarnings(3);
            defrSetTrackWarnings(3);
            defrSetUnitsWarnings(3);
            defrSetVersionWarnings(3);
            defrSetViaWarnings(3);
        }

        (void) defrSetOpenLogFileAppend();

        FILE* f = fopen(filename.c_str(), "r");
        if (!f)
        {
            std::cerr << "Could not open input file " << filename << "\n";
            return false;
        }
        // set defDB
        defDB = &m_db;
        defDriver = this;
        // Set case sensitive to 0 to start with, in History & PropertyDefinition
        // reset it to 1.
        int res = defrRead(f, filename.c_str(), userData, 1);
        // reset to NULL
        defDB = NULL;
        defDriver = NULL;

        if (res)
            //limboPrint(limbo::kERROR, "Reader returns bad status.\n", filename.c_str());

            (void)defrPrintUnusedCallbacks(stdout);
        (void)defrReleaseNResetMemory();
        (void)defrUnsetNonDefaultCbk();
        (void)defrUnsetNonDefaultStartCbk();
        (void)defrUnsetNonDefaultEndCbk();
        // Unset all the callbacks
        defrUnsetArrayNameCbk ();
        defrUnsetAssertionCbk ();
        defrUnsetAssertionsStartCbk ();
        defrUnsetAssertionsEndCbk ();
        defrUnsetBlockageCbk ();
        defrUnsetBlockageStartCbk ();
        defrUnsetBlockageEndCbk ();
        defrUnsetBusBitCbk ();
        defrUnsetCannotOccupyCbk ();
        defrUnsetCanplaceCbk ();
        defrUnsetCaseSensitiveCbk ();
        defrUnsetComponentCbk ();
        defrUnsetComponentExtCbk ();
        defrUnsetComponentStartCbk ();
        defrUnsetComponentEndCbk ();
        defrUnsetConstraintCbk ();
        defrUnsetConstraintsStartCbk ();
        defrUnsetConstraintsEndCbk ();
        defrUnsetDefaultCapCbk ();
        defrUnsetDesignCbk ();
        defrUnsetDesignEndCbk ();
        defrUnsetDieAreaCbk ();
        defrUnsetDividerCbk ();
        defrUnsetExtensionCbk ();
        defrUnsetFillCbk ();
        defrUnsetFillStartCbk ();
        defrUnsetFillEndCbk ();
        defrUnsetFPCCbk ();
        defrUnsetFPCStartCbk ();
        defrUnsetFPCEndCbk ();
        defrUnsetFloorPlanNameCbk ();
        defrUnsetGcellGridCbk ();
        defrUnsetGroupCbk ();
        defrUnsetGroupExtCbk ();
        defrUnsetGroupMemberCbk ();
        defrUnsetComponentMaskShiftLayerCbk ();
        defrUnsetGroupNameCbk ();
        defrUnsetGroupsStartCbk ();
        defrUnsetGroupsEndCbk ();
        defrUnsetHistoryCbk ();
        defrUnsetIOTimingCbk ();
        defrUnsetIOTimingsStartCbk ();
        defrUnsetIOTimingsEndCbk ();
        defrUnsetIOTimingsExtCbk ();
        defrUnsetNetCbk ();
        defrUnsetNetNameCbk ();
        defrUnsetNetNonDefaultRuleCbk ();
        defrUnsetNetConnectionExtCbk ();
        defrUnsetNetExtCbk ();
        defrUnsetNetPartialPathCbk ();
        defrUnsetNetSubnetNameCbk ();
        defrUnsetNetStartCbk ();
        defrUnsetNetEndCbk ();
        defrUnsetNonDefaultCbk ();
        defrUnsetNonDefaultStartCbk ();
        defrUnsetNonDefaultEndCbk ();
        defrUnsetPartitionCbk ();
        defrUnsetPartitionsExtCbk ();
        defrUnsetPartitionsStartCbk ();
        defrUnsetPartitionsEndCbk ();
        defrUnsetPathCbk ();
        defrUnsetPinCapCbk ();
        defrUnsetPinCbk ();
        defrUnsetPinEndCbk ();
        defrUnsetPinExtCbk ();
        defrUnsetPinPropCbk ();
        defrUnsetPinPropStartCbk ();
        defrUnsetPinPropEndCbk ();
        defrUnsetPropCbk ();
        defrUnsetPropDefEndCbk ();
        defrUnsetPropDefStartCbk ();
        defrUnsetRegionCbk ();
        defrUnsetRegionStartCbk ();
        defrUnsetRegionEndCbk ();
        defrUnsetRowCbk ();
        defrUnsetScanChainExtCbk ();
        defrUnsetScanchainCbk ();
        defrUnsetScanchainsStartCbk ();
        defrUnsetScanchainsEndCbk ();
        defrUnsetSiteCbk ();
        defrUnsetSlotCbk ();
        defrUnsetSlotStartCbk ();
        defrUnsetSlotEndCbk ();
        defrUnsetSNetWireCbk ();
        defrUnsetSNetCbk ();
        defrUnsetSNetStartCbk ();
        defrUnsetSNetEndCbk ();
        defrUnsetSNetPartialPathCbk ();
        defrUnsetStartPinsCbk ();
        defrUnsetStylesCbk ();
        defrUnsetStylesStartCbk ();
        defrUnsetStylesEndCbk ();
        defrUnsetTechnologyCbk ();
        defrUnsetTimingDisableCbk ();
        defrUnsetTimingDisablesStartCbk ();
        defrUnsetTimingDisablesEndCbk ();
        defrUnsetTrackCbk ();
        defrUnsetUnitsCbk ();
        defrUnsetVersionCbk ();
        defrUnsetVersionStrCbk ();
        defrUnsetViaCbk ();
        defrUnsetViaExtCbk ();
        defrUnsetViaStartCbk ();
        defrUnsetViaEndCbk ();

        // Release allocated singleton data.
        defrClear();
        fclose(f);

        return true;
    }

    bool read(DefDataBase& db, const string& defFile)
    {
        Driver driver (db);
        //driver.trace_scanning = true;
        //driver.trace_parsing = true;

        return driver.parse_file(defFile);
    }

} // namespace example

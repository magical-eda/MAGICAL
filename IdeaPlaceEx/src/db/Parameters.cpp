#include "Parameters.h"

PROJECT_NAMESPACE_BEGIN
Parameters::Parameters()
{
    _ifUsePinAssignment = true;
    _numThreads = 10;
    _gridStep = -1;
    _virtualBoundaryExtension = 200; ///< The extension of current virtual boundary to the bounding box of placement
    _virtualPinInterval = 400; ///< The interval between each virtual pin
    _layoutOffset = 1000; ///< The default offset for the placement
    _defaultAspectRatio = 1.2;
    _maxWhiteSpace = 2;
    _defaultSignalFlowWeight = 10;
    _defaultCurrentFlowWeight = 0.5;
    _defaultRelativeRatioOfPowerNet = 0.2;
}
PROJECT_NAMESPACE_END

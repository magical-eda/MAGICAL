#include "lefViaImpl.hpp"
#include "lefVia.hpp"

PROJECT_NAMESPACE_START

bool LefViaImplementor::configureMetalWidthHeight(
    Int_t botWidth, Int_t botHeight, Int_t topWidth, Int_t topHeight)
{
  if (_checkBotWidth)
  {
    if (! _metalWidthConfig.checkBotWidthHeight(botWidth, botHeight, 0))
    {
      return false;
    }
  }
  if (_checkTopWidth)
  {
    if (! _metalWidthConfig.checkBotWidthHeight(topWidth, topHeight, 1))
    {
      return false;
    }
  }
  Int_t botEncloX = -1;
  Int_t botEncloY = -1;
  Int_t topEncloX = -1;
  Int_t topEncloY = -1;
  if (botWidth > 0)
  {
    botEncloX = (botWidth - _pLefVia->_cutBBox.width()) / 2;
    if (botEncloX < 0) { return false; }
  }
  if (botHeight > 0)
  {
    botEncloY = (botHeight - _pLefVia->_cutBBox.height()) / 2;
    if (botEncloY < 0) { return false; }
  }
  if (topWidth > 0)
  {
    topEncloX = (topWidth - _pLefVia->_cutBBox.width()) / 2;
    if (topEncloX < 0) { return false; }
  }
  if (topHeight > 0)
  {
    topEncloY = (topHeight - _pLefVia->_cutBBox.height()) / 2;
    if (topEncloY < 0) { return false; }
  }
  if (_checkBotEnclosure)
  {
    _metalEnclosureConfig.legalize(botEncloX, botEncloY, 0);
  }
  if (_checkTopEnclosure)
  {
    _metalEnclosureConfig.legalize(topEncloX, topEncloY, 0);
  }
  // Configure the metals
  auto botBox = _pLefVia->_cutBBox;
  botBox.expandX(botEncloX);
  botBox.expandY(botEncloY);
  _pLefVia->_vBoxes.at(0).clear();
  _pLefVia->_vBoxes.at(0).emplace_back(botBox);
  auto topBox = _pLefVia->_cutBBox;
  topBox.expandX(topEncloX);
  topBox.expandY(topEncloY);
  _pLefVia->_vBoxes.at(2).clear();
  _pLefVia->_vBoxes.at(2).emplace_back(topBox);
  return true;
}

bool LefViaImplementor::configureMetalWithHeightWithDirection(Orient2D_t botOrient, Orient2D_t topOrient)
{
  Int_t botWidth, botHeight, topWidth, topHeight;
  switch (botOrient)
  {
    case Orient2D_t::VERTICAL : botWidth = _pLefVia->_cutBBox.width(); botHeight =  -1; break;
    case Orient2D_t::HORIZONTAL : botWidth = -1; botHeight =  _pLefVia->_cutBBox.height(); break;
    default: configureDefault(); return true;
  }
  switch (topOrient)
  {
    case Orient2D_t::VERTICAL : topWidth = _pLefVia->_cutBBox.width(); topHeight =  -1; break;
    case Orient2D_t::HORIZONTAL : topWidth = -1; topHeight =  _pLefVia->_cutBBox.height(); break;
    default: configureDefault(); return true;
  }
  return configureMetalWidthHeight(botWidth, botHeight, topWidth, topHeight);
}

void LefViaImplementor::configureDefault()
{
  Int_t botEnclosure = 0;
  if (_checkBotEnclosure)
  {
    botEnclosure = _metalEnclosureConfig._enclosureOverhang1.at(0);
  }
  Int_t topEnclosure = 0;
  if (_checkTopEnclosure)
  {
    topEnclosure = _metalEnclosureConfig._enclosureOverhang1.at(1);
  }
  // Use the same enclosure
  auto botBox = _pLefVia->_cutBBox;
  botBox.expand(botEnclosure);
  _pLefVia->_vBoxes.at(0).clear();
  _pLefVia->_vBoxes.at(0).emplace_back(botBox);
  auto topBox = _pLefVia->_cutBBox;
  topBox.expand(topEnclosure);
  _pLefVia->_vBoxes.at(2).clear();
  _pLefVia->_vBoxes.at(2).emplace_back(topBox);
}

PROJECT_NAMESPACE_END

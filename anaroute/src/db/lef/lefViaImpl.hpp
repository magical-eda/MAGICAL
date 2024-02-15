/**
 * @file   lefViaImpl.hpp
 * @brief  The configurable LefVia
 * @author Keren Zhu
 * @date   02/12/2020
 *
 **/

#ifndef DB_LEF_VIA_IMPL_HPP_
#define DB_LEF_VIA_IMPL_HPP_

#include "src/db/lef/lefViaRule.hpp"

PROJECT_NAMESPACE_START

class LefVia; 
/// @brief the class for the via configuration rule with two enclusre rule
class LefViaEnclosureConfig
{
  friend class LefViaImplementor;
  public:
    explicit LefViaEnclosureConfig() = default;
    explicit LefViaEnclosureConfig(const std::array<Int_t, 2> &enclosureOverhang1, const std::array<Int_t, 2> &enclosureOverhang2)
      : _enclosureOverhang1(enclosureOverhang1), _enclosureOverhang2(enclosureOverhang2) {}
    void configure(const std::array<Int_t, 2> &enclosureOverhang1, const std::array<Int_t, 2> &enclosureOverhang2)
    {
      _enclosureOverhang1 = enclosureOverhang1;
      _enclosureOverhang2 = enclosureOverhang2;
    }
    /// @param < 0 if don't care
    void legalize(Int_t &edgeEnclosure1, Int_t &edgeEnclosure2, Int_t layer)
    {
      Int_t *larger = edgeEnclosure1 > edgeEnclosure2 ? &edgeEnclosure1 : &edgeEnclosure2;
      Int_t *smaller =  edgeEnclosure1 <= edgeEnclosure2 ? &edgeEnclosure1 : &edgeEnclosure2;
      if (*smaller < 0)
      {
        std::swap(larger, smaller);
      }
      *larger = std::max(*larger, _enclosureOverhang1.at(layer));
      *smaller = std::max(*smaller, _enclosureOverhang2.at(layer));
    }
  protected:
      std::array<Int_t, 2> _enclosureOverhang1 = {0, 0};
      std::array<Int_t, 2> _enclosureOverhang2 = {0, 0};
};

/// @brief the class for the via configuration rule with two enclusre rule
class LefViaWidthConfig
{
  friend class LefViaImplementor;
  public:
    explicit LefViaWidthConfig() = default;
    explicit LefViaWidthConfig(const std::array<Int_t, 2> &widthLo, const std::array<Int_t, 2> &widthHi)
      : _widthLo(widthLo), _widthHi(widthHi) {}
    void configure(const std::array<Int_t, 2> &widthLo, const std::array<Int_t, 2> &widthHi)
    {
      _widthLo = widthLo;
      _widthHi = widthHi;
    }
    bool checkBotWidthHeight(Int_t width, Int_t height, Int_t layer)
    {
      if (width > 0)
      {
        if (width > _widthHi.at(layer))
        {
          return false;
        }
        if (width < _widthLo.at(layer))
        {
          return false;
        }
      }
      if (height > 0)
      {
        if (height > _widthHi.at(layer))
        {
          return false;
        }
        if (height < _widthLo.at(layer))
        {
          return false;
        }
      }
      return true;
    }
  protected:
      std::array<Int_t, 2> _widthLo = {0, 0};
      std::array<Int_t, 2> _widthHi = {0, 0};
};



class LefViaImplementor 
{
  public:
    explicit LefViaImplementor() = default;
    explicit LefViaImplementor(LefVia *lefVia)
    { _pLefVia = lefVia; }
    /// @brief configure the width and height for the via metals
    /// @param first: target width for bottom metal. -1 if don't care
    /// @param second: target height for bottom metal. -1 if don't care
    /// @param third: target width for top metal. -1 if don't care
    /// @param fourth: target height for top metal. -1 if don't care
    /// @return true: if configuration success. false: failed. the original shapes should not change
    bool configureMetalWidthHeight(Int_t botWidth, Int_t botHeight, Int_t topWidth, Int_t topHeight);
    /// @brief shortcut to the adjust metal width/height. Use minimum size for the direction and adjust the other side correspondingly
    /// @param Orientation 2D type representing the routing direction
    /// @return if configuration success
    bool configureMetalWithHeightWithDirection(Orient2D_t botOrient, Orient2D_t topOrient);
    /// @brief configure the via into default mode
    void configureDefault();
    /* Config */
    LefViaEnclosureConfig &metalEnclosureConfig() { return _metalEnclosureConfig; }
    LefViaWidthConfig &metalWidthConfig() { return _metalWidthConfig; }
    void setCheckBotWidth(bool checkBotWidth) { _checkBotWidth = checkBotWidth; }
    void setCheckTopWidth(bool checkTopWidth) { _checkTopEnclosure = checkTopWidth; }
    void setCheckBotEnclosure(bool checkBotEnclosure) { _checkBotEnclosure = checkBotEnclosure; }
    void setCheckTopEnclosure(bool checkTopEnclosure) { _checkTopEnclosure = checkTopEnclosure; }
  protected:
    LefVia *_pLefVia = nullptr;
    LefViaEnclosureConfig _metalEnclosureConfig;
    LefViaWidthConfig _metalWidthConfig;
    bool _checkBotWidth = false;
    bool _checkTopWidth = false;
    bool _checkBotEnclosure = false;
    bool _checkTopEnclosure = false;
};


/// @brief The traits for viarules
template<typename ViaRuleType>
struct LefViaImplementorConceptTraits
{
  typedef typename ViaRuleType::LefViaImplType ImplType;
  static inline void configure(const ViaRuleType &viaRule, ImplType &impl)
  {
    viaRule.config(impl);
  }
};


/// @brief custom LefViaImplementorConceptTraits for ViaRuleTemplate1
template<>
struct LefViaImplementorConceptTraits<LefViaRuleTemplate1>
{
  static inline void configure(const LefViaRuleTemplate1 &viaRule, LefViaImplementor &impl)
  {
    impl.metalEnclosureConfig().configure(viaRule.enclosureOverhang1, viaRule.enclosureOverhang2);
    impl.metalWidthConfig().configure(viaRule.widthLo, viaRule.widthHi);
    impl.setCheckBotWidth(true);
    impl.setCheckTopWidth(true);
    impl.setCheckBotEnclosure(true);
    impl.setCheckTopEnclosure(true);
  }
};

PROJECT_NAMESPACE_END

#endif //DB_LEF_VIA_IMPL_HPP_

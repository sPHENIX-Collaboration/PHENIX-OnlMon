#ifndef MUI_CHANNEL_ID_H
#define MUI_CHANNEL_ID_H

#include <MuiCommon.hh>

#include <cstddef>
#include <cstdio>
#include <iostream>

/**
 * Contains the "software" representation of a MuID channel address.
 *
 * Indexing a MuID readout channel (in the "software" scheme) requires
 * five numbers:
 \begin{enumerate}
 \item the arm number, fArm (0 - South, 1 - North)
 \item the gap or plane number in the arm, fPlane (0-4)
 \item the panel number within the plane, fPanel (0-5)
 \item the tube orientation, fOrientation
       ("kHORIZ" - horizontal, "kVERT" - vertical)
 \item the tube number within the panel for the given orientation,
       fTwoPack (0 - some panel-dependent value)
 \end{enumerate}
 *
 * @author Kyle Pope \URL{mailto:pope@phy.ornl.gov}
 *
 */

class TMuiChannelId
  {
  public:
    // PUBLIC CONSTANTS =================================================
    /// Number of gaps in each arm.
    static const short kPlanesPerArm = 5;
    /// Number of panels in a gap.
    static const short kPanelsPerPlane = 6;
    /// Maximum number of two-packs in a given orientation/gap.
    static const short kTwoPacksPerPlaneMax = 326;
    /// Number of two-packs of a single orientation in a panel.
    // (This number differs from H to V and from panel to panel, so the
    // upper limit is used here.)
    static const short kTwoPacksPerPanelMax = 64;

    /// Number of arms in the system.
    static const short kArmsTotal = 1;
    static const short kOrientations = 2;

    /// Number of gaps total in the system.
    static const short kPlanesTotal
    = TMuiChannelId::kArmsTotal*TMuiChannelId::kPlanesPerArm; //5
    /// Number of panels total in the system.
    static const short kPanelsTotal
    = TMuiChannelId::kPlanesTotal*TMuiChannelId::kPanelsPerPlane; // 30
    /// = kPanelsTotal * kOrientations
    static const short kPanelsXYTotal
    = TMuiChannelId::kPanelsTotal*TMuiChannelId::kOrientations; //60
    /// Number of two-pack orientations = 2 (HORIZ, VERT).

    /// Upper limit on the number of two-packs in the system.
    //  (assuming each panel has kTwoPacksPerPanelMax two-packs)
    static const short kTwoPacksMaxTotal
    = TMuiChannelId::kPanelsXYTotal*TMuiChannelId::kTwoPacksPerPanelMax; //3840
    /// Maximum allowed raw hits per panel.
    static const short kHitsPerPanelMax
    = TMuiChannelId::kTwoPacksPerPanelMax; //64
    /// Maximum allowed raw hits in entire MuID.
    static const short kHitsMaxTotal
    = TMuiChannelId::kTwoPacksMaxTotal; //3840
    // CONSTRUCTOR/DESTRUCTOR METHODS ===================================

    /// Constructor.
    TMuiChannelId(const short& arm = -1,
                  const short& plane = -1,
                  const short& panel = -1,
                  const EOrient_t& orient = kHORIZ,
                  const short& twopack = -1)
        : fArm(arm), fPlane(plane), fPanel(panel), fOrient(orient),
        fTwoPack(twopack)
    {}
    ;

    /// Copy constructor.
    TMuiChannelId(const TMuiChannelId& orig)
        : fArm(orig.fArm), fPlane(orig.fPlane), fPanel(orig.fPanel),
        fOrient(orig.fOrient), fTwoPack(orig.fTwoPack)
    {}
    ;

    /// Assignment operator.
    TMuiChannelId& operator=(const TMuiChannelId& orig);

    /// Destructor.
    virtual ~TMuiChannelId();

    // ACCESSOR METHODS =================================================
    /// Arm identifier (0 - South, 1 - North)
    short Arm() const
      {
        return fArm;
      };
    /// Gap identifier (0-4)
    short Plane() const
      {
        return fPlane;
      };
    /// Panel identifier (0-5)
    short Panel() const
      {
        return fPanel;
      };
    /// Orientation of two-pack (kHORIZ or kVERT).
    EOrient_t Orient() const
      {
        return fOrient;
      };
    /// Two-pack ident (starts at 0)
    short TwoPack() const
      {
        return fTwoPack;
      };

    // MUTATOR METHODS ==================================================
    void Show();

    /// Set the sub-identifiers.
    void Set(const short& arm, const short& plane,
             const short& panel, const EOrient_t& orient = kHORIZ,
             const short& twopack = -1);

    int Set(int);

    /// Set the sub-identifiers.
    void SetID(const short& arm, const short& plane,
               const short& panel, const short& orient = kHORIZ,
               const short& twopack = -1);

  private:
    short fArm;         // arm identifier (0 - South, 1 - North)
    short fPlane;       // plane (gap) identifier (0-4)
    short fPanel;       // panel identifier (0-5)
    EOrient_t fOrient;  // panel orientation (kHORIZ or kVERT)
    short fTwoPack;     // two-pack identifier (0-123?)
  };

// Destructor.
inline TMuiChannelId::~TMuiChannelId()
{}

inline TMuiChannelId& TMuiChannelId::operator=(const TMuiChannelId& orig)
{
  // Assignment operator.
  if (this != &orig)
    {             // Watch out for self-assignment!
      fArm = orig.fArm;
      fPlane = orig.fPlane;
      fPanel = orig.fPanel;
      fOrient = orig.fOrient;
      fTwoPack = orig.fTwoPack;
    }
  return *this;
}

inline void TMuiChannelId::Set(const short& arm,
                               const short& plane,
                               const short& panel,
                               const EOrient_t& orient,
                               const short& twopack)
{
  fArm = arm;
  fPlane = plane;
  fPanel = panel;
  fOrient = orient;
  fTwoPack = twopack;
}

inline void TMuiChannelId::SetID(const short& arm,
                                 const short& plane,
                                 const short& panel,
                                 const short& orient,
                                 const short& twopack)
{
  fArm = arm;
  fPlane = plane;
  fPanel = panel;
  fTwoPack = twopack;
  if (orient == kVERT)
    {
      fOrient = kVERT;
    }
  else
    {
      fOrient = kHORIZ;
    }
}

inline void TMuiChannelId::Show()
{
  printf(" TMuiChannelId: Arm = %d Plane = %d  Panel = %d  Orientation = %d TwoPack = %d\n",(int)fArm, (int)fPlane,(int)fPanel,(int)fOrient,(int)fTwoPack);
}

/// Print TMuiChannelId information to a stream.
std::ostream& operator << (std::ostream& s, const TMuiChannelId& n);

/// Return the position of a TMuiChannelId object in the PanelGeo hash table.
//size_t PanelHash(const TMuiChannelId& ident);
short PanelHash(const TMuiChannelId& ident);

/// Return the position of a TMuiChannelId object in the TwoPackGeo hash table.
//size_t TwoPackHash(const TMuiChannelId& ident);
short TwoPackHash(const TMuiChannelId& ident);

#endif  /* MUI_CHANNEL_ID_H */

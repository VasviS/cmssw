#ifndef JetReco_BasicJet_h
#define JetReco_BasicJet_h

/** \class BasicJet
 *
 * \short Jets made from CaloTowers
 *
 * BasicJet represents generic Jets witjout
 * any specific information
 * in addition to generic Jet parameters
 *
 * \author Fedor Ratnikov, UMd
 *
 * \version   $Id: BasicJet.h,v 1.16 2006/06/27 23:15:06 fedor Exp $
 ************************************************************/


#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/BasicJetfwd.h"

namespace reco {
class BasicJet : public Jet {
 public:
  
  /** Default constructor*/
  BasicJet() {}
  
  /** Constructor from values*/
  BasicJet(const LorentzVector& fP4, const Point& fVertex);
  
  virtual ~BasicJet() {};
  
  /// Polymorphic clone
  virtual BasicJet* clone () const;
  
 private:
  /// Polymorphic overlap
  virtual bool overlap( const Candidate & ) const;
  
};
}
#endif

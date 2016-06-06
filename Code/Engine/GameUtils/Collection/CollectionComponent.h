#pragma once

#include <GameUtils/Basics.h>
#include <Core/World/World.h>
#include <Core/World/Component.h>
#include <GameUtils/Collection/CollectionResource.h>

typedef ezComponentManager<class ezCollectionComponent> ezCollectionComponentManager;

class EZ_GAMEUTILS_DLL ezCollectionComponent : public ezComponent
{
  EZ_DECLARE_COMPONENT_TYPE(ezCollectionComponent, ezComponent, ezCollectionComponentManager);

public:
  ezCollectionComponent();

  virtual void SerializeComponent(ezWorldWriter& stream) const override;
  virtual void DeserializeComponent(ezWorldReader& stream) override;

  void InitiatePreload();

  // ************************************* PROPERTIES ***********************************

  void SetCollectionFile(const char* szFile);
  const char* GetCollectionFile() const;

  void SetCollection(const ezCollectionResourceHandle& hPrefab);
  EZ_FORCE_INLINE const ezCollectionResourceHandle& GetCollection() const { return m_hCollection; }

  bool m_bPreloadAtStart;
  ezTime m_TimeToLoad;

protected:


  virtual void OnSimulationStarted() override;

  // ************************************* FUNCTIONS *****************************

private:

  ezCollectionResourceHandle m_hCollection;
};

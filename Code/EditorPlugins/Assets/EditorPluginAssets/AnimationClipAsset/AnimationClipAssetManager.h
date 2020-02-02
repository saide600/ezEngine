#pragma once

#include <EditorFramework/Assets/AssetDocumentManager.h>
#include <Foundation/Types/Status.h>

class ezAnimationClipAssetDocumentManager : public ezAssetDocumentManager
{
  EZ_ADD_DYNAMIC_REFLECTION(ezAnimationClipAssetDocumentManager, ezAssetDocumentManager);

public:
  ezAnimationClipAssetDocumentManager();
  ~ezAnimationClipAssetDocumentManager();

private:
  void OnDocumentManagerEvent(const ezDocumentManager::Event& e);

  virtual void InternalCreateDocument(const char* szDocumentTypeName, const char* szPath, bool bCreateNewDocument, ezDocument*& out_pDocument) override;
  virtual void InternalGetSupportedDocumentTypes(ezDynamicArray<const ezDocumentTypeDescriptor*>& inout_DocumentTypes) const override;

  virtual bool GeneratesProfileSpecificAssets() const override { return false; }

  ezAssetDocumentTypeDescriptor m_DocTypeDesc;
};


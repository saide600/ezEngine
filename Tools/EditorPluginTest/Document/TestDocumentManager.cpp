#include <PCH.h>
#include <EditorPluginTest/Document/TestDocumentManager.h>
#include <EditorPluginTest/Document/TestDocument.h>

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezTestDocumentManager, ezDocumentManagerBase, 1, ezRTTIDefaultAllocator<ezTestDocumentManager>);
EZ_END_DYNAMIC_REFLECTED_TYPE();

ezTestDocumentManager* ezTestDocumentManager::s_pSingleton = nullptr;

ezStatus ezTestDocumentManager::InternalCanOpenDocument(const char* szDocumentTypeName, const char* szFilePath) const
{
  return ezStatus(EZ_SUCCESS);
}

ezStatus ezTestDocumentManager::InternalCreateDocument(const char* szDocumentTypeName, const char* szPath, ezDocumentBase*& out_pDocument)
{
  ezStatus status;

  if (ezStringUtils::IsEqual(szDocumentTypeName, "ezScene"))
  {
    out_pDocument = new ezTestDocument(szPath);
  }
  else
  if (ezStringUtils::IsEqual(szDocumentTypeName, "ezPrefab"))
  {
    out_pDocument = new ezTestDocument(szPath);
  }
  else
  {
    status.m_sError = "Unknown Document Type";
  }

  if (out_pDocument)
  {
    status.m_Result = EZ_SUCCESS;
    //out_pDocument->SetFilePath(szPath);
  }

  return status;
}

void ezTestDocumentManager::InternalGetSupportedDocumentTypes(ezHybridArray<ezDocumentTypeDescriptor, 4>& out_DocumentTypes) const
{
  {
    ezDocumentTypeDescriptor td;
    td.m_bCanCreate = true;
    td.m_sDocumentTypeName = "ezScene";
    td.m_sFileExtensions.PushBack("ezScene");

    out_DocumentTypes.PushBack(td);
  }

  /*
  {
    ezDocumentTypeDescriptor td;
    td.m_bCanCreate = true;
    td.m_sDocumentTypeName = "ezPrefab";
    td.m_sFileExtensions.PushBack("prefab");

    out_DocumentTypes.PushBack(td);
  }
  */
}




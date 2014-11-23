#include <ToolsFoundation/PCH.h>
#include <ToolsFoundation/Object/DocumentObjectManager.h>
#include <ToolsFoundation/Document/Document.h>
#include <Foundation/IO/MemoryStream.h>

ezDocumentObjectManagerBase::ezDocumentObjectManagerBase(const ezDocumentBase* pDocument) : m_pDocument(pDocument)
{
}

ezDocumentObjectBase* ezDocumentObjectManagerBase::CreateObject(ezReflectedTypeHandle hType, ezUuid guid)
{
  ezDocumentObjectBase* pObject = InternalCreateObject(hType);

  if (pObject)
  {
    if (!hType.GetType()->GetDefaultInitialization().IsEmpty())
    {
      ezMemoryStreamStorage storage;
      ezMemoryStreamWriter writer(&storage);
      ezMemoryStreamReader reader(&storage);
      writer.WriteBytes(hType.GetType()->GetDefaultInitialization().GetData(), hType.GetType()->GetDefaultInitialization().GetElementCount());

      ezToolsReflectionUtils::ReadObjectPropertiesFromJSON(reader, pObject->GetTypeAccessor());
    }

    if (guid.IsValid())
      pObject->m_Guid = guid;
    else
      pObject->m_Guid.CreateNewUuid();
  }

  return pObject;
}

void ezDocumentObjectManagerBase::DestroyObject(ezDocumentObjectBase* pObject)
{
  for (ezDocumentObjectBase* pChild : pObject->m_Children)
  {
    DestroyObject(pChild);
  }

  InternalDestroyObject(pObject);
}

bool ezDocumentObjectManagerBase::CanAdd(ezReflectedTypeHandle hType, const ezDocumentObjectBase* pParent) const
{
  if (pParent == nullptr)
    pParent = m_pDocument->GetObjectTree()->GetRootObject();

  // Test whether parent exists in tree.
  if (pParent != m_pDocument->GetObjectTree()->GetRootObject())
  {  
    const ezDocumentObjectBase* pObjectInTree = m_pDocument->GetObjectTree()->GetObject(pParent->GetGuid());

    if (pObjectInTree == nullptr)
      return false;

    EZ_ASSERT(pObjectInTree == pParent, "Tree Corruption!!!");
  }

  return InternalCanAdd(hType, pParent);
}

bool ezDocumentObjectManagerBase::CanRemove(const ezDocumentObjectBase* pObject) const
{
  const ezDocumentObjectBase* pObjectInTree = m_pDocument->GetObjectTree()->GetObject(pObject->GetGuid());

  if (pObjectInTree == nullptr)
    return false;

  EZ_ASSERT(pObjectInTree == pObject, "Tree Corruption!!!");

  return InternalCanRemove(pObject);
}

bool ezDocumentObjectManagerBase::CanMove(const ezDocumentObjectBase* pObject, const ezDocumentObjectBase* pNewParent, ezInt32 iChildIndex) const
{
  if (pNewParent == nullptr)
    pNewParent = m_pDocument->GetObjectTree()->GetRootObject();

  if (pObject == pNewParent)
    return false;

  const ezDocumentObjectBase* pObjectInTree = m_pDocument->GetObjectTree()->GetObject(pObject->GetGuid());

  if (pObjectInTree == nullptr)
    return false;

  EZ_ASSERT(pObjectInTree == pObject, "Tree Corruption!!!");

  if (pNewParent != m_pDocument->GetObjectTree()->GetRootObject())
  {
    const ezDocumentObjectBase* pNewParentInTree = m_pDocument->GetObjectTree()->GetObject(pNewParent->GetGuid());

    if (pNewParentInTree == nullptr)
      return false;

    EZ_ASSERT(pNewParentInTree == pNewParent, "Tree Corruption!!!");
  }

  const ezDocumentObjectBase* pCurParent = pNewParent->GetParent();

  while (pCurParent)
  {
    if (pCurParent == pObject)
      return false;

    pCurParent = pCurParent->GetParent();
  }

  if (iChildIndex < 0)
    iChildIndex = pNewParent->GetChildren().GetCount();

  if ((ezUInt32)iChildIndex > pNewParent->GetChildren().GetCount())
    return false;

  if (pNewParent == pObject->GetParent())
  {
    // Test whether we are moving before or after ourselves, both of which are not allowed and would not change the tree.
    ezUInt32 iIndex = pObject->GetParent()->GetChildren().IndexOf((ezDocumentObjectBase*) pObject);
    if (iChildIndex == iIndex || iChildIndex == iIndex + 1)
      return false;
  }

  return InternalCanMove(pObject, pNewParent, iChildIndex);
}



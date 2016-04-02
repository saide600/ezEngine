#pragma once

#include <ToolsFoundation/Basics.h>
#include <EditorFramework/IPC/SyncObject.h>
#include <Foundation/Math/Mat4.h>
#include <Core/World/GameObject.h>

class ezWorld;
class ezMeshComponent;
class ezGizmo;

class EZ_EDITORFRAMEWORK_DLL ezGizmoHandle : public ezEditorEngineSyncObject
{
  EZ_ADD_DYNAMIC_REFLECTION(ezGizmoHandle, ezEditorEngineSyncObject);

public:
  ezGizmoHandle();

  ezGizmo* GetOwnerGizmo() const { return m_pParentGizmo; }

  void SetVisible(bool bVisible);

  void SetTransformation(const ezMat4& m);

  const ezMat4& GetTransformation() const { return m_Transformation; }

protected:
  bool m_bVisible;
  ezMat4 m_Transformation;

  void SetParentGizmo(ezGizmo* pParentGizmo) { m_pParentGizmo = pParentGizmo; }

private:
  ezGizmo* m_pParentGizmo;
};


enum ezEngineGizmoHandleType
{
  Arrow,
  Ring,
  Rect,
  Box,
  Piston,
  HalfPiston,
  Sphere,
};

class EZ_EDITORFRAMEWORK_DLL ezEngineGizmoHandle : public ezGizmoHandle
{
  EZ_ADD_DYNAMIC_REFLECTION(ezEngineGizmoHandle, ezGizmoHandle);

public:
  ezEngineGizmoHandle();
  ~ezEngineGizmoHandle();

  void Configure(ezGizmo* pParentGizmo, ezEngineGizmoHandleType type, const ezColor& col, bool bConstantSize = true, bool bForeground2 = false);

  bool IsSetupForEngine() const { return !m_hGameObject.IsInvalidated(); }

  bool SetupForEngine(ezWorld* pWorld, ezUInt32 uiNextComponentPickingID);
  void UpdateForEngine(ezWorld* pWorld);

protected:
  bool m_bConstantSize;
  bool m_bForeground2;
  ezInt32 m_iHandleType;
  ezGameObjectHandle m_hGameObject;
  ezMeshComponent* m_pMeshComponent;
  ezColor m_Color;
  ezWorld* m_pWorld;

private:

};


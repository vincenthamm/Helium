#pragma once

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/RendererInitialization.h"

#include "Windowing/WindowManager.h"

namespace Helium
{
	class Window;

	/// Renderer factory implementation for Windows.
	class HELIUM_FRAMEWORK_WIN_API RendererInitializationWin : public RendererInitialization
	{
	public:
		/// @name Renderer Initialization
		//@{
		virtual bool Initialize();
		//@}

		virtual void Shutdown();

		Window *GetMainWindow() { return m_pMainWindow; }

	private:
		/// Main application window.
		Window* m_pMainWindow;

		/// @name Window Callbacks
		//@{
		virtual void OnMainWindowDestroyed( Window* pWindow );
		//@}
	};
}

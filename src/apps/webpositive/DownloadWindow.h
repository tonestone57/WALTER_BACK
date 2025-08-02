/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef DOWNLOAD_WINDOW_H
#define DOWNLOAD_WINDOW_H


#include <String.h>
#include <Window.h>

#include <map>
#include <memory>

class BButton;
class BFile;
class BGroupLayout;
class BScrollView;
class BDownload;
class SettingsMessage;


class DownloadWindow : public BWindow {
public:
								DownloadWindow(BRect frame, bool visible,
									SettingsMessage* settings);
	virtual						~DownloadWindow();

	virtual	void				DispatchMessage(BMessage* message,
									BHandler* target);
	virtual void				FrameResized(float newWidth, float newHeight);
	virtual	void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested();

			bool				DownloadsInProgress();
			void				SetMinimizeOnClose(bool minimize);

private:
			void				_DownloadStarted(BDownload* download);
			void				_DownloadFinished(BDownload* download);
			void				_RemoveFinishedDownloads();
			void				_RemoveMissingDownloads();
			void				_ValidateButtonStatus();
			void				_SaveSettings();
			void				_SaveSettingsIfNeeded();
			void				_LoadSettings();
			bool				_OpenSettingsFile(BFile& file, uint32 mode);

private:
			std::unique_ptr<BMessageRunner>		fSaveSettingsRunner;
			BScrollView*		fDownloadsScrollView;
			BGroupLayout*		fDownloadViewsLayout;
			std::map<BString, std::unique_ptr<DownloadProgressView>> fDownloadViews;
			BButton*			fRemoveFinishedButton;
			BButton*			fRemoveMissingButton;
			BString				fDownloadPath;
			bool				fMinimizeOnClose;
};

#endif // DOWNLOAD_WINDOW_H

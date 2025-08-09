#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <Handler.h>
#include <Messenger.h>
#include <Path.h>
#include <Url.h>
#include <UrlRequest.h>
#include <Locker.h>

class BFile;

class BDownload : public BHandler, public BPrivate::Network::BUrlProtocolListener {
public:
								BDownload(const BUrl& url);
	virtual						~BDownload();

	// BUrlProtocolListener hooks
	virtual	void				DownloadProgress(BPrivate::Network::BUrlRequest* caller,
									off_t bytesReceived, off_t bytesTotal);
	virtual void				RequestCompleted(BPrivate::Network::BUrlRequest* caller,
									bool success);

	// BHandler hooks
	virtual void				MessageReceived(BMessage* message);

	// Public API
			const BUrl&			Url() const;
			const BPath&		Target() const;

			void				SetListener(const BMessenger& listener);

			status_t			Start(const BPath& target);
			void				Stop();

			void				Pause();
			void				Resume();
			bool				IsPaused() const;

private:
			BUrl				fUrl;
			BPath				fTarget;
			BFile*				fOutputFile;
			BMessenger			fListener;
			BPrivate::Network::BUrlRequest*		fRequest;
			bool				fPaused;
			off_t				fBytesReceived;
			BLocker				fLock;
};

#endif // DOWNLOAD_H

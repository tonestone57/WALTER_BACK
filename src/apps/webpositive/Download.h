#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <Handler.h>
#include <Path.h>
#include <Url.h>
#include <UrlRequest.h>
#include <private/netservices/UrlProtocolListener.h>


class BDownload : public BHandler, public BUrlProtocolListener {
public:
								BDownload(const BUrl& url);
	virtual						~BDownload();

	// BUrlProtocolListener hooks
	virtual	void				DownloadProgress(BUrlRequest* caller,
									off_t bytesReceived, off_t bytesTotal);
	virtual void				RequestCompleted(BUrlRequest* caller,
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
			BMessenger			fListener;
			BUrlRequest*		fRequest;
			bool				fPaused;
			off_t				fBytesReceived;
};

#endif // DOWNLOAD_H

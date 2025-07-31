#include "Download.h"

#include <Messenger.h>
#include <private/netservices/UrlContext.h>


BDownload::BDownload(const BUrl& url)
	:
	BHandler(),
	BUrlProtocolListener(),
	fUrl(url),
	fRequest(NULL),
	fPaused(false),
	fBytesReceived(0)
{
}


BDownload::~BDownload()
{
	if (fRequest)
		fRequest->Stop();
}


void
BDownload::DownloadProgress(BUrlRequest* caller, off_t bytesReceived,
	off_t bytesTotal)
{
	if (fListener.IsValid()) {
		BMessage progress(B_DOWNLOAD_PROGRESS);
		progress.AddInt64("current size", bytesReceived);
		progress.AddInt64("expected size", bytesTotal);
		fListener.SendMessage(&progress);
	}
	fBytesReceived = bytesReceived;
}


void
BDownload::RequestCompleted(BUrlRequest* caller, bool success)
{
	if (fListener.IsValid()) {
		BMessage completed(B_DOWNLOAD_REMOVED);
		completed.AddPointer("download", this);
		fListener.SendMessage(&completed);
	}
	fRequest = NULL;
	delete this;
}


void
BDownload::MessageReceived(BMessage* message)
{
	BHandler::MessageReceived(message);
}


const BUrl&
BDownload::Url() const
{
	return fUrl;
}


const BPath&
BDownload::Target() const
{
	return fTarget;
}


void
BDownload::SetListener(const BMessenger& listener)
{
	fListener = listener;
}


status_t
BDownload::Start(const BPath& target)
{
	fTarget = target;
	BPrivate::Network::BUrlContext* context = new BPrivate::Network::BUrlContext();
	fRequest = BUrlRequest::RequestUrl(fUrl, this, context);
	if (!fRequest) {
		delete context;
		return B_ERROR;
	}
	return B_OK;
}


void
BDownload::Stop()
{
	if (fRequest)
		fRequest->Stop();
}


void
BDownload::Pause()
{
	fPaused = true;
	// fRequest->Pause(); // BUrlRequest doesn't have Pause()
}


void
BDownload::Resume()
{
	fPaused = false;
	// fRequest->Resume(); // BUrlRequest doesn't have Resume()
}


bool
BDownload::IsPaused() const
{
	return fPaused;
}

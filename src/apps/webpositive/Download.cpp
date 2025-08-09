#include "Download.h"

#include <File.h>
#include <WebPage.h>
#include <UrlProtocolRoster.h>
#include <private/netservices/UrlContext.h>
#include <private/netservices/UrlProtocolDispatchingListener.h>


BDownload::BDownload(const BUrl& url)
	:
	BHandler(),
	BPrivate::Network::BUrlProtocolListener(),
	fUrl(url),
	fOutputFile(NULL),
	fRequest(NULL),
	fPaused(false),
	fBytesReceived(0)
{
}


#include <Autolock.h>


BDownload::~BDownload()
{
	BAutolock lock(fLock);
	if (fRequest)
		fRequest->Stop();
	delete fRequest;
	delete fOutputFile;
}


void
BDownload::DownloadProgress(BPrivate::Network::BUrlRequest* caller,
	off_t bytesReceived, off_t bytesTotal)
{
	if (fListener.IsValid()) {
		BMessage progress(BPrivate::Network::B_URL_PROTOCOL_DOWNLOAD_PROGRESS);
		progress.AddInt64("current size", bytesReceived);
		progress.AddInt64("expected size", bytesTotal);
		fListener.SendMessage(&progress);
	}
	fBytesReceived = bytesReceived;
}


void
BDownload::RequestCompleted(BPrivate::Network::BUrlRequest* caller,
	bool success)
{
	if (fListener.IsValid()) {
		BMessage completed(B_DOWNLOAD_REMOVED);
		completed.AddPointer("download", this);
		fListener.SendMessage(&completed);
	}
	BAutolock lock(fLock);
	delete fRequest;
	fRequest = NULL;
	delete fOutputFile;
	fOutputFile = NULL;
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
	fOutputFile = new BFile(fTarget.Path(), B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
	if (fOutputFile->InitCheck() != B_OK) {
		delete fOutputFile;
		fOutputFile = NULL;
		return B_ERROR;
	}

	BAutolock lock(fLock);
	BPrivate::Network::BUrlContext* context = new BPrivate::Network::BUrlContext();
	fRequest = BPrivate::Network::BUrlProtocolRoster::MakeRequest(fUrl, fOutputFile, this, context);
	if (!fRequest) {
		delete context;
		delete fOutputFile;
		fOutputFile = NULL;
		return B_ERROR;
	}
	return B_OK;
}


void
BDownload::Stop()
{
	BAutolock lock(fLock);
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

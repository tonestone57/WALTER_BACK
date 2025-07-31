#ifndef WEB_DOWNLOAD_H
#define WEB_DOWNLOAD_H

#include <support/Url.h>
#include <private/netservices/UrlRequest.h>
#include <private/netservices/UrlProtocolListener.h>
#include <private/netservices/UrlContext.h>

class BWebDownload : public BUrlRequest {
public:
    BWebDownload(const BUrl& url, BUrlProtocolListener* listener,
        BUrlContext* context)
        :
        BUrlRequest(url, listener, context),
        fPaused(false)
    {
    }

    void Pause()
    {
        fPaused = true;
    }

    void Resume()
    {
        fPaused = false;
    }

    bool IsPaused() const
    {
        return fPaused;
    }

private:
    bool fPaused;
};

#endif // WEB_DOWNLOAD_H

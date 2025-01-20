/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the MIT license.
 * Constributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#include "MainWindow.h"

#include <Application.h>

#include <Url.h>
#include <private/netservices/UrlProtocolDispatchingListener.h>
#include <private/netservices2/HttpSession.h>
#include <private/netservices2/HttpRequest.h>
#include <private/netservices2/HttpResult.h>
#include <private/netservices2/NetServicesDefs.h>

#include <LayoutBuilder.h>
#include <ScrollBar.h>
#include <String.h>
#include <Window.h>
#include <TextView.h>

#include <iostream>

using namespace BPrivate::Network;

MainWindow::MainWindow()
	:	BWindow(BRect(100,100,500,400),"litebrowser",
                B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE),
	fHtmlView(NULL),
	fDataReceived()
{
	fHtmlView = new LiteHtmlView(Bounds().InsetBySelf(B_V_SCROLL_BAR_WIDTH, B_H_SCROLL_BAR_HEIGHT), "html");
    fScrollView = new BScrollView("htmlScrollview", fHtmlView, 0, true, true);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0.0)
        .Add(fScrollView);

    ResizeTo(640.0, 860.0);
    CenterOnScreen();

    BSize min = fScrollView->MinSize();
	BSize max = fScrollView->MaxSize();
	fScrollView->SetExplicitMinSize(min);
	fScrollView->SetExplicitMaxSize(max);

    fScrollBarHorizontal = fScrollView->ScrollBar(B_HORIZONTAL);
    fScrollBarVertical   = fScrollView->ScrollBar(B_VERTICAL);

    fHtmlView->StartWatching(this, M_HTML_RENDERED);
    fHtmlView->StartWatching(this, M_ANCHOR_CLICKED);
}

void MainWindow::Load(const char* filePathOrUrl, const char* masterStylesPath, const char* userStylesPath)
{
    fHtmlView->RenderUrl(filePathOrUrl, masterStylesPath, userStylesPath);
}

void
MainWindow::MessageReceived(BMessage *msg)
{
	int32 originalWhat;
	int8 protocolWhat;
	BString str;
	switch (msg->what)
	{
		case B_URL_PROTOCOL_NOTIFICATION:
		{
			std::cout << "  UrlRequest update received" << std::endl;
			// data loading, request finished, or download progress
			if (B_OK == msg->FindInt8("be:urlProtocolMessageType",&protocolWhat))
			{
				switch (protocolWhat)
				{
					case B_URL_PROTOCOL_DOWNLOAD_PROGRESS:
					{
						std::cout << "  Data received" << std::endl;
						if (B_OK == msg->FindString("url:data",&str))
						{
							fDataReceived += str;
						}
						break;
					}
					case B_URL_PROTOCOL_REQUEST_COMPLETED:
					{
						std::cout << "  BUrlRequest complete" << std::endl;

                        fHtmlView->RenderHtml(fDataReceived);    // FIXME: not needed, at least for local files
                        fDataReceived.Truncate(0);
                        UpdateScrollBars();

						break;
					}
					default:
					{
						// ignore
					}
				}
			}
			break;
		}
		case B_OBSERVER_NOTICE_CHANGE:
		{
			if (B_OK == msg->FindInt32(B_OBSERVE_ORIGINAL_WHAT,&originalWhat))
			{
				switch (originalWhat)
				{
					case M_HTML_RENDERED:
					{
						std::cout << "HTML_RENDERED received" << std::endl;
                        UpdateScrollBars();
                        //fHtmlView->Invalidate();
					}
                    case M_ANCHOR_CLICKED:
                    {
                        const char *href = msg->FindString("href");
                        std::cout << "Anchor clicked received: " << href << std::endl;
                        BPoint anchorPos;
                        status_t result = msg->FindPoint("fragmentPos", &anchorPos);
                        if (result == B_OK) {
                            std::cout << "   scrolling to anchor with y pos = " << anchorPos.y << std::endl;
                            fHtmlView->ScrollTo(0, anchorPos.y);
                        }
                    }
					default:
					{
						break;
					}
				}
			}
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

void
MainWindow::ScreenChanged(BRect	screenSize, color_space	depth)
{
    UpdateScrollBars();
	fHtmlView->Invalidate();
}

void
MainWindow::FrameResized(float newWidth, float newHeight)
{
    UpdateScrollBars();
	fHtmlView->Invalidate();
}

void
MainWindow::UpdateScrollBars()
{
    BSize viewSize(fScrollView->PreferredSize());
    float width, height;
    fHtmlView->GetPreferredSize(&width, &height);

    fScrollBarHorizontal->SetRange(0,  width);
    fScrollBarHorizontal->SetSteps(10, width / 10);
    fScrollBarVertical->SetRange(0,  height);
    fScrollBarVertical->SetSteps(10, height / 10);
}

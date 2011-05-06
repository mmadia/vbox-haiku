/* $Id$ */

/** @file
 * VBox WDDM Miniport driver
 *
 * Contains base definitions of constants & structures used
 * to control & perform rendering,
 * such as DMA commands types, allocation types, escape codes, etc.
 * used by both miniport & display drivers.
 *
 * The latter uses these and only these defs to communicate with the former
 * by posting appropriate requests via D3D RT Krnl Svc accessing callbacks.
 */

/*
 * Copyright (C) 2011 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef ___VBoxMPIf_h___
#define ___VBoxMPIf_h___

#include <VBox/VBoxVideo.h>
#include "../../../../include/VBoxDisplay.h"
#include <VBox/VBoxUhgsmi.h>

/* One would increase this whenever definitions in this file are changed */
#define VBOXVIDEOIF_VERSION 9

/* create allocation func */
typedef enum
{
    VBOXWDDM_ALLOC_TYPE_UNEFINED = 0,
    VBOXWDDM_ALLOC_TYPE_STD_SHAREDPRIMARYSURFACE,
    VBOXWDDM_ALLOC_TYPE_STD_SHADOWSURFACE,
    VBOXWDDM_ALLOC_TYPE_STD_STAGINGSURFACE,
    /* this one is win 7-specific and hence unused for now */
    VBOXWDDM_ALLOC_TYPE_STD_GDISURFACE
    /* custom allocation types requested from user-mode d3d module will go here */
    , VBOXWDDM_ALLOC_TYPE_UMD_RC_GENERIC
    , VBOXWDDM_ALLOC_TYPE_UMD_HGSMI_BUFFER
} VBOXWDDM_ALLOC_TYPE;

/* usage */
typedef enum
{
    VBOXWDDM_ALLOCUSAGE_TYPE_UNEFINED = 0,
    /* set for the allocation being primary */
    VBOXWDDM_ALLOCUSAGE_TYPE_PRIMARY,
} VBOXWDDM_ALLOCUSAGE_TYPE;

typedef struct VBOXWDDM_SURFACE_DESC
{
    UINT width;
    UINT height;
    D3DDDIFORMAT format;
    UINT bpp;
    UINT pitch;
    UINT depth;
    UINT slicePitch;
    UINT cbSize;
    D3DDDI_VIDEO_PRESENT_SOURCE_ID VidPnSourceId;
    D3DDDI_RATIONAL RefreshRate;
} VBOXWDDM_SURFACE_DESC, *PVBOXWDDM_SURFACE_DESC;

typedef struct VBOXWDDM_ALLOCINFO
{
    VBOXWDDM_ALLOC_TYPE enmType;
    union
    {
        struct
        {
            D3DDDI_RESOURCEFLAGS fFlags;
            uint64_t hSharedHandle;
            VBOXWDDM_SURFACE_DESC SurfDesc;
        };

        struct
        {
            uint32_t cbBuffer;
            uint64_t hSynch;
            VBOXUHGSMI_SYNCHOBJECT_TYPE enmSynchType;
        };
    };
} VBOXWDDM_ALLOCINFO, *PVBOXWDDM_ALLOCINFO;

/* this resource is OpenResource'd rather than CreateResource'd */
#define VBOXWDDM_RESOURCE_F_OPENNED      0x00000001
/* identifies this is a resource created with CreateResource, the VBOXWDDMDISP_RESOURCE::fRcFlags is valid */
#define VBOXWDDM_RESOURCE_F_TYPE_GENERIC 0x00000002

typedef struct VBOXWDDM_RC_DESC
{
    D3DDDI_RESOURCEFLAGS fFlags;
    D3DDDIFORMAT enmFormat;
    D3DDDI_POOL enmPool;
    D3DDDIMULTISAMPLE_TYPE enmMultisampleType;
    UINT MultisampleQuality;
    UINT MipLevels;
    UINT Fvf;
    D3DDDI_VIDEO_PRESENT_SOURCE_ID VidPnSourceId;
    D3DDDI_RATIONAL RefreshRate;
    D3DDDI_ROTATION enmRotation;
} VBOXWDDM_RC_DESC, *PVBOXWDDM_RC_DESC;

typedef struct VBOXWDDM_RCINFO
{
    uint32_t fFlags;
    VBOXWDDM_RC_DESC RcDesc;
    uint32_t cAllocInfos;
//    VBOXWDDM_ALLOCINFO aAllocInfos[1];
} VBOXWDDM_RCINFO, *PVBOXWDDM_RCINFO;

typedef struct VBOXWDDM_DMA_PRIVATEDATA_FLAFS
{
    union
    {
        struct
        {
            UINT bCmdInDmaBuffer : 1;
            UINT bReserved : 31;
        };
        uint32_t Value;
    };
} VBOXWDDM_DMA_PRIVATEDATA_FLAFS, *PVBOXWDDM_DMA_PRIVATEDATA_FLAFS;

typedef struct VBOXWDDM_DMA_PRIVATEDATA_BASEHDR
{
    VBOXVDMACMD_TYPE enmCmd;
    union
    {
        VBOXWDDM_DMA_PRIVATEDATA_FLAFS fFlags;
        uint32_t u32CmdReserved;
    };
} VBOXWDDM_DMA_PRIVATEDATA_BASEHDR, *PVBOXWDDM_DMA_PRIVATEDATA_BASEHDR;

typedef struct VBOXWDDM_UHGSMI_BUFFER_UI_SUBMIT_INFO
{
    VBOXUHGSMI_BUFFER_SUBMIT_FLAGS fSubFlags;
    uint32_t offData;
    uint32_t cbData;
} VBOXWDDM_UHGSMI_BUFFER_UI_SUBMIT_INFO, *PVBOXWDDM_UHGSMI_BUFFER_UI_SUBMIT_INFO;

typedef struct VBOXWDDM_DMA_PRIVATEDATA_UM_CHROMIUM_CMD
{
    VBOXWDDM_DMA_PRIVATEDATA_BASEHDR Base;
    VBOXWDDM_UHGSMI_BUFFER_UI_SUBMIT_INFO aBufInfos[1];
} VBOXWDDM_DMA_PRIVATEDATA_UM_CHROMIUM_CMD, *PVBOXWDDM_DMA_PRIVATEDATA_UM_CHROMIUM_CMD;

#define VBOXVHWA_F_ENABLED  0x00000001
#define VBOXVHWA_F_CKEY_DST 0x00000002
#define VBOXVHWA_F_CKEY_SRC 0x00000004

#define VBOXVHWA_MAX_FORMATS 8

typedef struct VBOXVHWA_INFO
{
    uint32_t fFlags;
    uint32_t cOverlaysSupported;
    uint32_t cFormats;
    D3DDDIFORMAT aFormats[VBOXVHWA_MAX_FORMATS];
} VBOXVHWA_INFO;

#define VBOXWDDM_OVERLAY_F_CKEY_DST      0x00000001
#define VBOXWDDM_OVERLAY_F_CKEY_DSTRANGE 0x00000002
#define VBOXWDDM_OVERLAY_F_CKEY_SRC      0x00000004
#define VBOXWDDM_OVERLAY_F_CKEY_SRCRANGE 0x00000008
#define VBOXWDDM_OVERLAY_F_BOB           0x00000010
#define VBOXWDDM_OVERLAY_F_INTERLEAVED   0x00000020
#define VBOXWDDM_OVERLAY_F_MIRROR_LR     0x00000040
#define VBOXWDDM_OVERLAY_F_MIRROR_UD     0x00000080
#define VBOXWDDM_OVERLAY_F_DEINTERLACED  0x00000100

typedef struct VBOXWDDM_OVERLAY_DESC
{
    uint32_t fFlags;
    UINT DstColorKeyLow;
    UINT DstColorKeyHigh;
    UINT SrcColorKeyLow;
    UINT SrcColorKeyHigh;
} VBOXWDDM_OVERLAY_DESC, *PVBOXWDDM_OVERLAY_DESC;

/* the dirty rect info is valid */
#define VBOXWDDM_DIRTYREGION_F_VALID      0x00000001
#define VBOXWDDM_DIRTYREGION_F_RECT_VALID 0x00000002

typedef struct VBOXWDDM_DIRTYREGION
{
    uint32_t fFlags; /* <-- see VBOXWDDM_DIRTYREGION_F_xxx flags above */
    RECT Rect;
} VBOXWDDM_DIRTYREGION, *PVBOXWDDM_DIRTYREGION;

typedef struct VBOXWDDM_OVERLAY_INFO
{
    VBOXWDDM_OVERLAY_DESC OverlayDesc;
    VBOXWDDM_DIRTYREGION DirtyRegion; /* <- the dirty region of the overlay surface */
} VBOXWDDM_OVERLAY_INFO, *PVBOXWDDM_OVERLAY_INFO;

typedef struct VBOXWDDM_OVERLAYFLIP_INFO
{
    VBOXWDDM_DIRTYREGION DirtyRegion; /* <- the dirty region of the overlay surface */
} VBOXWDDM_OVERLAYFLIP_INFO, *PVBOXWDDM_OVERLAYFLIP_INFO;


typedef enum
{
    VBOXWDDM_CONTEXT_TYPE_UNDEFINED = 0,
    VBOXWDDM_CONTEXT_TYPE_SYSTEM,
    VBOXWDDM_CONTEXT_TYPE_CUSTOM_3D,
    VBOXWDDM_CONTEXT_TYPE_CUSTOM_2D,
    VBOXWDDM_CONTEXT_TYPE_CUSTOM_UHGSMI_3D,
    VBOXWDDM_CONTEXT_TYPE_CUSTOM_UHGSMI_GL,
    VBOXWDDM_CONTEXT_TYPE_CUSTOM_SESSION
} VBOXWDDM_CONTEXT_TYPE;

typedef struct VBOXWDDM_CREATECONTEXT_INFO
{
    /* interface version, i.e. 9 for d3d9, 8 for d3d8, etc. */
    uint32_t u32IfVersion;
    /* true if d3d false if ddraw */
    VBOXWDDM_CONTEXT_TYPE enmType;
    /* we use uint64_t instead of HANDLE to ensure structure def is the same for both 32-bit and 64-bit
     * since x64 kernel driver can be called by 32-bit UMD */
    uint64_t hUmEvent;
    /* info to be passed to UMD notification to identify the context */
    uint64_t u64UmInfo;
} VBOXWDDM_CREATECONTEXT_INFO, *PVBOXWDDM_CREATECONTEXT_INFO;

typedef uint64_t VBOXDISP_UMHANDLE;
typedef uint32_t VBOXDISP_KMHANDLE;

typedef struct VBOXWDDM_RECTS_FLAFS
{
    union
    {
        struct
        {
            /* used only in conjunction with bSetVisibleRects.
             * if set - VBOXWDDM_RECTS_INFO::aRects[0] contains view rectangle */
            UINT bSetViewRect : 1;
            /* sets visible regions */
            UINT bSetVisibleRects : 1;
            /* adds hidden regions */
            UINT bAddHiddenRects : 1;
            /* hide entire window */
            UINT bHide : 1;
            /* reserved */
            UINT Reserved : 28;
        };
        uint32_t Value;
    };
} VBOXWDDM_RECTS_FLAFS, *PVBOXWDDM_RECTS_FLAFS;

typedef struct VBOXWDDM_RECTS_INFO
{
    uint32_t cRects;
    RECT aRects[1];
} VBOXWDDM_RECTS_INFO, *PVBOXWDDM_RECTS_INFO;

#define VBOXWDDM_RECTS_INFO_SIZE4CRECTS(_cRects) (RT_OFFSETOF(VBOXWDDM_RECTS_INFO, aRects[(_cRects)]))
#define VBOXWDDM_RECTS_INFO_SIZE(_pRects) (VBOXVIDEOCM_CMD_RECTS_SIZE4CRECTS((_pRects)->cRects))

typedef struct VBOXVIDEOCM_CMD_HDR
{
    uint64_t u64UmData;
    uint32_t cbCmd;
    uint32_t u32CmdSpecific;
}VBOXVIDEOCM_CMD_HDR, *PVBOXVIDEOCM_CMD_HDR;

AssertCompile((sizeof (VBOXVIDEOCM_CMD_HDR) & 7) == 0);

typedef struct VBOXVIDEOCM_CMD_RECTS
{
    VBOXWDDM_RECTS_FLAFS fFlags;
    VBOXWDDM_RECTS_INFO RectsInfo;
} VBOXVIDEOCM_CMD_RECTS, *PVBOXVIDEOCM_CMD_RECTS;

typedef struct VBOXVIDEOCM_CMD_RECTS_INTERNAL
{
    union
    {
        VBOXDISP_UMHANDLE hSwapchainUm;
        uint64_t u64Alignment;
    };
    VBOXVIDEOCM_CMD_RECTS Cmd;
} VBOXVIDEOCM_CMD_RECTS_INTERNAL, *PVBOXVIDEOCM_CMD_RECTS_INTERNAL;

#define VBOXVIDEOCM_CMD_RECTS_INTERNAL_SIZE4CRECTS(_cRects) (RT_OFFSETOF(VBOXVIDEOCM_CMD_RECTS_INTERNAL, Cmd.RectsInfo.aRects[(_cRects)]))
#define VBOXVIDEOCM_CMD_RECTS_INTERNAL_SIZE(_pCmd) (VBOXVIDEOCM_CMD_RECTS_INTERNAL_SIZE4CRECTS((_pCmd)->cRects))

typedef struct VBOXWDDM_GETVBOXVIDEOCMCMD_HDR
{
    uint32_t cbCmdsReturned;
    uint32_t cbRemainingCmds;
    uint32_t cbRemainingFirstCmd;
    uint32_t u32Reserved;
} VBOXWDDM_GETVBOXVIDEOCMCMD_HDR, *PVBOXWDDM_GETVBOXVIDEOCMCMD_HDR;

typedef struct VBOXDISPIFESCAPE_GETVBOXVIDEOCMCMD
{
    VBOXDISPIFESCAPE EscapeHdr;
    VBOXWDDM_GETVBOXVIDEOCMCMD_HDR Hdr;
} VBOXDISPIFESCAPE_GETVBOXVIDEOCMCMD, *PVBOXDISPIFESCAPE_GETVBOXVIDEOCMCMD;

AssertCompile((sizeof (VBOXDISPIFESCAPE_GETVBOXVIDEOCMCMD) & 7) == 0);
AssertCompile(RT_OFFSETOF(VBOXDISPIFESCAPE_GETVBOXVIDEOCMCMD, EscapeHdr) == 0);

typedef struct VBOXDISPIFESCAPE_DBGPRINT
{
    VBOXDISPIFESCAPE EscapeHdr;
    /* null-terminated string to DbgPrint including \0 */
    char aStringBuf[1];
} VBOXDISPIFESCAPE_DBGPRINT, *PVBOXDISPIFESCAPE_DBGPRINT;
AssertCompile(RT_OFFSETOF(VBOXDISPIFESCAPE_DBGPRINT, EscapeHdr) == 0);

typedef struct VBOXSCREENLAYOUT_ELEMENT
{
    D3DDDI_VIDEO_PRESENT_SOURCE_ID VidPnSourceId;
    POINT pos;
} VBOXSCREENLAYOUT_ELEMENT, *PVBOXSCREENLAYOUT_ELEMENT;

typedef struct VBOXSCREENLAYOUT
{
    uint32_t cScreens;
    VBOXSCREENLAYOUT_ELEMENT aScreens[1];
} VBOXSCREENLAYOUT, *PVBOXSCREENLAYOUT;

typedef struct VBOXDISPIFESCAPE_SCREENLAYOUT
{
    VBOXDISPIFESCAPE EscapeHdr;
    VBOXSCREENLAYOUT ScreenLayout;
} VBOXDISPIFESCAPE_SCREENLAYOUT, *PVBOXDISPIFESCAPE_SCREENLAYOUT;

typedef struct VBOXSWAPCHAININFO
{
    VBOXDISP_KMHANDLE hSwapchainKm; /* in, NULL if new is being created */
    VBOXDISP_UMHANDLE hSwapchainUm; /* in, UMD private data */
    RECT Rect;
    UINT u32Reserved;
    UINT cAllocs;
    D3DKMT_HANDLE ahAllocs[1];
}VBOXSWAPCHAININFO, *PVBOXSWAPCHAININFO;
typedef struct VBOXDISPIFESCAPE_SWAPCHAININFO
{
    VBOXDISPIFESCAPE EscapeHdr;
    VBOXSWAPCHAININFO SwapchainInfo;
} VBOXDISPIFESCAPE_SWAPCHAININFO, *PVBOXDISPIFESCAPE_SWAPCHAININFO;

typedef struct VBOXVIDEOCM_UM_ALLOC
{
    VBOXDISP_KMHANDLE hAlloc;
    uint32_t cbData;
    uint64_t pvData;
    uint64_t hSynch;
    VBOXUHGSMI_SYNCHOBJECT_TYPE enmSynchType;
} VBOXVIDEOCM_UM_ALLOC, *PVBOXVIDEOCM_UM_ALLOC;

typedef struct VBOXDISPIFESCAPE_UHGSMI_ALLOCATE
{
    VBOXDISPIFESCAPE EscapeHdr;
    VBOXVIDEOCM_UM_ALLOC Alloc;
} VBOXDISPIFESCAPE_UHGSMI_ALLOCATE, *PVBOXDISPIFESCAPE_UHGSMI_ALLOCATE;

typedef struct VBOXDISPIFESCAPE_UHGSMI_DEALLOCATE
{
    VBOXDISPIFESCAPE EscapeHdr;
    VBOXDISP_KMHANDLE hAlloc;
} VBOXDISPIFESCAPE_UHGSMI_DEALLOCATE, *PVBOXDISPIFESCAPE_UHGSMI_DEALLOCATE;

typedef struct VBOXWDDM_UHGSMI_BUFFER_UI_INFO_ESCAPE
{
    VBOXDISP_KMHANDLE hAlloc;
    VBOXWDDM_UHGSMI_BUFFER_UI_SUBMIT_INFO Info;
} VBOXWDDM_UHGSMI_BUFFER_UI_INFO_ESCAPE, *PVBOXWDDM_UHGSMI_BUFFER_UI_INFO_ESCAPE;

typedef struct VBOXDISPIFESCAPE_UHGSMI_SUBMIT
{
    VBOXDISPIFESCAPE EscapeHdr;
    VBOXWDDM_UHGSMI_BUFFER_UI_INFO_ESCAPE aBuffers[1];
} VBOXDISPIFESCAPE_UHGSMI_SUBMIT, *PVBOXDISPIFESCAPE_UHGSMI_SUBMIT;

/* query info func */
typedef struct VBOXWDDM_QI
{
    uint32_t u32Version;
    uint32_t cInfos;
    VBOXVHWA_INFO aInfos[VBOX_VIDEO_MAX_SCREENS];
} VBOXWDDM_QI;

/* submit cmd func */

/* tooling */
DECLINLINE(UINT) vboxWddmCalcBitsPerPixel(D3DDDIFORMAT format)
{
    switch (format)
    {
        case D3DDDIFMT_R8G8B8:
            return 24;
        case D3DDDIFMT_A8R8G8B8:
        case D3DDDIFMT_X8R8G8B8:
            return 32;
        case D3DDDIFMT_R5G6B5:
        case D3DDDIFMT_X1R5G5B5:
        case D3DDDIFMT_A1R5G5B5:
        case D3DDDIFMT_A4R4G4B4:
            return 16;
        case D3DDDIFMT_R3G3B2:
        case D3DDDIFMT_A8:
            return 8;
        case D3DDDIFMT_A8R3G3B2:
        case D3DDDIFMT_X4R4G4B4:
            return 16;
        case D3DDDIFMT_A2B10G10R10:
        case D3DDDIFMT_A8B8G8R8:
        case D3DDDIFMT_X8B8G8R8:
        case D3DDDIFMT_G16R16:
        case D3DDDIFMT_A2R10G10B10:
            return 32;
        case D3DDDIFMT_A16B16G16R16:
            return 64;
        case D3DDDIFMT_A8P8:
            return 16;
        case D3DDDIFMT_P8:
        case D3DDDIFMT_L8:
            return 8;
        case D3DDDIFMT_A8L8:
            return 16;
        case D3DDDIFMT_A4L4:
            return 8;
        case D3DDDIFMT_V8U8:
        case D3DDDIFMT_L6V5U5:
            return 16;
        case D3DDDIFMT_X8L8V8U8:
        case D3DDDIFMT_Q8W8V8U8:
        case D3DDDIFMT_V16U16:
        case D3DDDIFMT_W11V11U10:
        case D3DDDIFMT_A2W10V10U10:
            return 32;
        case D3DDDIFMT_D16_LOCKABLE:
        case D3DDDIFMT_D16:
        case D3DDDIFMT_D15S1:
            return 16;
        case D3DDDIFMT_D32:
        case D3DDDIFMT_D24S8:
        case D3DDDIFMT_D24X8:
        case D3DDDIFMT_D24X4S4:
        case D3DDDIFMT_D24FS8:
        case D3DDDIFMT_D32_LOCKABLE:
        case D3DDDIFMT_D32F_LOCKABLE:
            return 32;
        case D3DDDIFMT_S8_LOCKABLE:
            return 8;
        default:
            AssertBreakpoint();
            return 0;
    }
}

DECLINLINE(uint32_t) vboxWddmFormatToFourcc(D3DDDIFORMAT format)
{
    uint32_t uFormat = (uint32_t)format;
    /* assume that in case both four bytes are non-zero, this is a fourcc */
    if ((format & 0xff000000)
            && (format & 0x00ff0000)
            && (format & 0x0000ff00)
            && (format & 0x000000ff)
            )
        return uFormat;
    return 0;
}

#define VBOXWDDM_ROUNDBOUND(_v, _b) (((_v) + ((_b) - 1)) & ~((_b) - 1))

DECLINLINE(UINT) vboxWddmCalcPitch(UINT w, UINT bitsPerPixel)
{
    UINT Pitch = bitsPerPixel * w;
    /* pitch is now in bits, translate in bytes */
    return VBOXWDDM_ROUNDBOUND(Pitch, 8) >> 3;
}

DECLINLINE(void) vboxWddmRectUnite(RECT *pR, const RECT *pR2Unite)
{
    pR->left = RT_MIN(pR->left, pR2Unite->left);
    pR->top = RT_MIN(pR->top, pR2Unite->top);
    pR->right = RT_MAX(pR->right, pR2Unite->right);
    pR->bottom = RT_MAX(pR->bottom, pR2Unite->bottom);
}

DECLINLINE(bool) vboxWddmRectIntersection(const RECT *a, const RECT *b, RECT *rect)
{
    Assert(a);
    Assert(b);
    Assert(rect);
    rect->left = RT_MAX(a->left, b->left);
    rect->right = RT_MIN(a->right, b->right);
    rect->top = RT_MAX(a->top, b->top);
    rect->bottom = RT_MIN(a->bottom, b->bottom);
    return (rect->right>rect->left) && (rect->bottom>rect->top);
}

DECLINLINE(bool) vboxWddmRectIsEqual(const RECT *pRect1, const RECT *pRect2)
{
    Assert(pRect1);
    Assert(pRect2);
    if (pRect1->left != pRect2->left)
        return false;
    if (pRect1->top != pRect2->top)
        return false;
    if (pRect1->right != pRect2->right)
        return false;
    if (pRect1->bottom != pRect2->bottom)
        return false;
    return true;
}

DECLINLINE(bool) vboxWddmRectIsCoveres(const RECT *pRect, const RECT *pCovered)
{
    Assert(pRect);
    Assert(pCovered);
    if (pRect->left > pCovered->left)
        return false;
    if (pRect->top > pCovered->top)
        return false;
    if (pRect->right < pCovered->right)
        return false;
    if (pRect->bottom < pCovered->bottom)
        return false;
    return true;
}

DECLINLINE(bool) vboxWddmRectIsEmpty(const RECT * pRect)
{
    return pRect->left == pRect->right-1 && pRect->top == pRect->bottom-1;
}

DECLINLINE(bool) vboxWddmRectIsIntersect(const RECT * pRect1, const RECT * pRect2)
{
    return !((pRect1->left < pRect2->left && pRect1->right < pRect2->left)
            || (pRect2->left < pRect1->left && pRect2->right < pRect1->left)
            || (pRect1->top < pRect2->top && pRect1->bottom < pRect2->top)
            || (pRect2->top < pRect1->top && pRect2->bottom < pRect1->top));
}

DECLINLINE(void) vboxWddmRectUnited(RECT * pDst, const RECT * pRect1, const RECT * pRect2)
{
    pDst->left = RT_MIN(pRect1->left, pRect2->left);
    pDst->top = RT_MIN(pRect1->top, pRect2->top);
    pDst->right = RT_MAX(pRect1->right, pRect2->right);
    pDst->bottom = RT_MAX(pRect1->bottom, pRect2->bottom);
}

DECLINLINE(void) vboxWddmRectTranslate(RECT * pRect, int x, int y)
{
    pRect->left   += x;
    pRect->top    += y;
    pRect->right  += x;
    pRect->bottom += y;
}

DECLINLINE(void) vboxWddmRectMove(RECT * pRect, int x, int y)
{
    LONG w = pRect->right - pRect->left;
    LONG h = pRect->bottom - pRect->top;
    pRect->left   = x;
    pRect->top    = y;
    pRect->right  = w + x;
    pRect->bottom = h + y;
}

DECLINLINE(void) vboxWddmRectTranslated(RECT *pDst, const RECT * pRect, int x, int y)
{
    *pDst = *pRect;
    vboxWddmRectTranslate(pDst, x, y);
}

DECLINLINE(void) vboxWddmRectMoved(RECT *pDst, const RECT * pRect, int x, int y)
{
    *pDst = *pRect;
    vboxWddmRectMove(pDst, x, y);
}

DECLINLINE(void) vboxWddmDirtyRegionAddRect(PVBOXWDDM_DIRTYREGION pInfo, const RECT *pRect)
{
    if (!(pInfo->fFlags & VBOXWDDM_DIRTYREGION_F_VALID))
    {
        pInfo->fFlags = VBOXWDDM_DIRTYREGION_F_VALID;
        if (pRect)
        {
            pInfo->fFlags |= VBOXWDDM_DIRTYREGION_F_RECT_VALID;
            pInfo->Rect = *pRect;
        }
    }
    else if (!!(pInfo->fFlags & VBOXWDDM_DIRTYREGION_F_RECT_VALID))
    {
        if (pRect)
            vboxWddmRectUnite(&pInfo->Rect, pRect);
        else
            pInfo->fFlags &= ~VBOXWDDM_DIRTYREGION_F_RECT_VALID;
    }
}

DECLINLINE(void) vboxWddmDirtyRegionUnite(PVBOXWDDM_DIRTYREGION pInfo, const PVBOXWDDM_DIRTYREGION pInfo2)
{
    if (pInfo2->fFlags & VBOXWDDM_DIRTYREGION_F_VALID)
    {
        if (pInfo2->fFlags & VBOXWDDM_DIRTYREGION_F_RECT_VALID)
            vboxWddmDirtyRegionAddRect(pInfo, &pInfo2->Rect);
        else
            vboxWddmDirtyRegionAddRect(pInfo, NULL);
    }
}

DECLINLINE(void) vboxWddmDirtyRegionClear(PVBOXWDDM_DIRTYREGION pInfo)
{
    pInfo->fFlags = 0;
}

#endif /* #ifndef ___VBoxMPIf_h___ */
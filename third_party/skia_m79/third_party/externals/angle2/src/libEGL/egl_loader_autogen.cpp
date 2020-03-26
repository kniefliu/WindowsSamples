// GENERATED FILE - DO NOT EDIT.
// Generated by generate_loader.py using data from egl.xml and egl_angle_ext.xml.
//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// egl_loader_autogen.cpp:
//   Simple EGL function loader.

#include "egl_loader_autogen.h"

PFNEGLCHOOSECONFIGPROC EGL_ChooseConfig;
PFNEGLCOPYBUFFERSPROC EGL_CopyBuffers;
PFNEGLCREATECONTEXTPROC EGL_CreateContext;
PFNEGLCREATEPBUFFERSURFACEPROC EGL_CreatePbufferSurface;
PFNEGLCREATEPIXMAPSURFACEPROC EGL_CreatePixmapSurface;
PFNEGLCREATEWINDOWSURFACEPROC EGL_CreateWindowSurface;
PFNEGLDESTROYCONTEXTPROC EGL_DestroyContext;
PFNEGLDESTROYSURFACEPROC EGL_DestroySurface;
PFNEGLGETCONFIGATTRIBPROC EGL_GetConfigAttrib;
PFNEGLGETCONFIGSPROC EGL_GetConfigs;
PFNEGLGETCURRENTDISPLAYPROC EGL_GetCurrentDisplay;
PFNEGLGETCURRENTSURFACEPROC EGL_GetCurrentSurface;
PFNEGLGETDISPLAYPROC EGL_GetDisplay;
PFNEGLGETERRORPROC EGL_GetError;
PFNEGLGETPROCADDRESSPROC EGL_GetProcAddress;
PFNEGLINITIALIZEPROC EGL_Initialize;
PFNEGLMAKECURRENTPROC EGL_MakeCurrent;
PFNEGLQUERYCONTEXTPROC EGL_QueryContext;
PFNEGLQUERYSTRINGPROC EGL_QueryString;
PFNEGLQUERYSURFACEPROC EGL_QuerySurface;
PFNEGLSWAPBUFFERSPROC EGL_SwapBuffers;
PFNEGLTERMINATEPROC EGL_Terminate;
PFNEGLWAITGLPROC EGL_WaitGL;
PFNEGLWAITNATIVEPROC EGL_WaitNative;
PFNEGLBINDTEXIMAGEPROC EGL_BindTexImage;
PFNEGLRELEASETEXIMAGEPROC EGL_ReleaseTexImage;
PFNEGLSURFACEATTRIBPROC EGL_SurfaceAttrib;
PFNEGLSWAPINTERVALPROC EGL_SwapInterval;
PFNEGLBINDAPIPROC EGL_BindAPI;
PFNEGLQUERYAPIPROC EGL_QueryAPI;
PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC EGL_CreatePbufferFromClientBuffer;
PFNEGLRELEASETHREADPROC EGL_ReleaseThread;
PFNEGLWAITCLIENTPROC EGL_WaitClient;
PFNEGLGETCURRENTCONTEXTPROC EGL_GetCurrentContext;
PFNEGLCREATESYNCPROC EGL_CreateSync;
PFNEGLDESTROYSYNCPROC EGL_DestroySync;
PFNEGLCLIENTWAITSYNCPROC EGL_ClientWaitSync;
PFNEGLGETSYNCATTRIBPROC EGL_GetSyncAttrib;
PFNEGLCREATEIMAGEPROC EGL_CreateImage;
PFNEGLDESTROYIMAGEPROC EGL_DestroyImage;
PFNEGLGETPLATFORMDISPLAYPROC EGL_GetPlatformDisplay;
PFNEGLCREATEPLATFORMWINDOWSURFACEPROC EGL_CreatePlatformWindowSurface;
PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC EGL_CreatePlatformPixmapSurface;
PFNEGLWAITSYNCPROC EGL_WaitSync;
PFNEGLSETBLOBCACHEFUNCSANDROIDPROC EGL_SetBlobCacheFuncsANDROID;
PFNEGLGETCOMPOSITORTIMINGANDROIDPROC EGL_GetCompositorTimingANDROID;
PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC EGL_GetCompositorTimingSupportedANDROID;
PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC EGL_GetFrameTimestampSupportedANDROID;
PFNEGLGETFRAMETIMESTAMPSANDROIDPROC EGL_GetFrameTimestampsANDROID;
PFNEGLGETNEXTFRAMEIDANDROIDPROC EGL_GetNextFrameIdANDROID;
PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC EGL_GetNativeClientBufferANDROID;
PFNEGLDUPNATIVEFENCEFDANDROIDPROC EGL_DupNativeFenceFDANDROID;
PFNEGLPRESENTATIONTIMEANDROIDPROC EGL_PresentationTimeANDROID;
PFNEGLCREATEDEVICEANGLEPROC EGL_CreateDeviceANGLE;
PFNEGLRELEASEDEVICEANGLEPROC EGL_ReleaseDeviceANGLE;
PFNEGLQUERYDISPLAYATTRIBANGLEPROC EGL_QueryDisplayAttribANGLE;
PFNEGLQUERYSTRINGIANGLEPROC EGL_QueryStringiANGLE;
PFNEGLPROGRAMCACHEGETATTRIBANGLEPROC EGL_ProgramCacheGetAttribANGLE;
PFNEGLPROGRAMCACHEPOPULATEANGLEPROC EGL_ProgramCachePopulateANGLE;
PFNEGLPROGRAMCACHEQUERYANGLEPROC EGL_ProgramCacheQueryANGLE;
PFNEGLPROGRAMCACHERESIZEANGLEPROC EGL_ProgramCacheResizeANGLE;
PFNEGLQUERYSURFACEPOINTERANGLEPROC EGL_QuerySurfacePointerANGLE;
PFNEGLCREATESTREAMPRODUCERD3DTEXTUREANGLEPROC EGL_CreateStreamProducerD3DTextureANGLE;
PFNEGLSTREAMPOSTD3DTEXTUREANGLEPROC EGL_StreamPostD3DTextureANGLE;
PFNEGLGETSYNCVALUESCHROMIUMPROC EGL_GetSyncValuesCHROMIUM;
PFNEGLQUERYDEVICEATTRIBEXTPROC EGL_QueryDeviceAttribEXT;
PFNEGLQUERYDEVICESTRINGEXTPROC EGL_QueryDeviceStringEXT;
PFNEGLQUERYDISPLAYATTRIBEXTPROC EGL_QueryDisplayAttribEXT;
PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC EGL_CreatePlatformPixmapSurfaceEXT;
PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC EGL_CreatePlatformWindowSurfaceEXT;
PFNEGLGETPLATFORMDISPLAYEXTPROC EGL_GetPlatformDisplayEXT;
PFNEGLDEBUGMESSAGECONTROLKHRPROC EGL_DebugMessageControlKHR;
PFNEGLLABELOBJECTKHRPROC EGL_LabelObjectKHR;
PFNEGLQUERYDEBUGKHRPROC EGL_QueryDebugKHR;
PFNEGLCLIENTWAITSYNCKHRPROC EGL_ClientWaitSyncKHR;
PFNEGLCREATESYNCKHRPROC EGL_CreateSyncKHR;
PFNEGLDESTROYSYNCKHRPROC EGL_DestroySyncKHR;
PFNEGLGETSYNCATTRIBKHRPROC EGL_GetSyncAttribKHR;
PFNEGLCREATEIMAGEKHRPROC EGL_CreateImageKHR;
PFNEGLDESTROYIMAGEKHRPROC EGL_DestroyImageKHR;
PFNEGLCREATESTREAMKHRPROC EGL_CreateStreamKHR;
PFNEGLDESTROYSTREAMKHRPROC EGL_DestroyStreamKHR;
PFNEGLQUERYSTREAMKHRPROC EGL_QueryStreamKHR;
PFNEGLQUERYSTREAMU64KHRPROC EGL_QueryStreamu64KHR;
PFNEGLSTREAMATTRIBKHRPROC EGL_StreamAttribKHR;
PFNEGLSTREAMCONSUMERACQUIREKHRPROC EGL_StreamConsumerAcquireKHR;
PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC EGL_StreamConsumerGLTextureExternalKHR;
PFNEGLSTREAMCONSUMERRELEASEKHRPROC EGL_StreamConsumerReleaseKHR;
PFNEGLSWAPBUFFERSWITHDAMAGEKHRPROC EGL_SwapBuffersWithDamageKHR;
PFNEGLWAITSYNCKHRPROC EGL_WaitSyncKHR;
PFNEGLPOSTSUBBUFFERNVPROC EGL_PostSubBufferNV;
PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALATTRIBSNVPROC EGL_StreamConsumerGLTextureExternalAttribsNV;

namespace angle
{
void LoadEGL_EGL(LoadProc loadProc)
{
    EGL_ChooseConfig  = reinterpret_cast<PFNEGLCHOOSECONFIGPROC>(loadProc("EGL_ChooseConfig"));
    EGL_CopyBuffers   = reinterpret_cast<PFNEGLCOPYBUFFERSPROC>(loadProc("EGL_CopyBuffers"));
    EGL_CreateContext = reinterpret_cast<PFNEGLCREATECONTEXTPROC>(loadProc("EGL_CreateContext"));
    EGL_CreatePbufferSurface =
        reinterpret_cast<PFNEGLCREATEPBUFFERSURFACEPROC>(loadProc("EGL_CreatePbufferSurface"));
    EGL_CreatePixmapSurface =
        reinterpret_cast<PFNEGLCREATEPIXMAPSURFACEPROC>(loadProc("EGL_CreatePixmapSurface"));
    EGL_CreateWindowSurface =
        reinterpret_cast<PFNEGLCREATEWINDOWSURFACEPROC>(loadProc("EGL_CreateWindowSurface"));
    EGL_DestroyContext = reinterpret_cast<PFNEGLDESTROYCONTEXTPROC>(loadProc("EGL_DestroyContext"));
    EGL_DestroySurface = reinterpret_cast<PFNEGLDESTROYSURFACEPROC>(loadProc("EGL_DestroySurface"));
    EGL_GetConfigAttrib =
        reinterpret_cast<PFNEGLGETCONFIGATTRIBPROC>(loadProc("EGL_GetConfigAttrib"));
    EGL_GetConfigs = reinterpret_cast<PFNEGLGETCONFIGSPROC>(loadProc("EGL_GetConfigs"));
    EGL_GetCurrentDisplay =
        reinterpret_cast<PFNEGLGETCURRENTDISPLAYPROC>(loadProc("EGL_GetCurrentDisplay"));
    EGL_GetCurrentSurface =
        reinterpret_cast<PFNEGLGETCURRENTSURFACEPROC>(loadProc("EGL_GetCurrentSurface"));
    EGL_GetDisplay     = reinterpret_cast<PFNEGLGETDISPLAYPROC>(loadProc("EGL_GetDisplay"));
    EGL_GetError       = reinterpret_cast<PFNEGLGETERRORPROC>(loadProc("EGL_GetError"));
    EGL_GetProcAddress = reinterpret_cast<PFNEGLGETPROCADDRESSPROC>(loadProc("EGL_GetProcAddress"));
    EGL_Initialize     = reinterpret_cast<PFNEGLINITIALIZEPROC>(loadProc("EGL_Initialize"));
    EGL_MakeCurrent    = reinterpret_cast<PFNEGLMAKECURRENTPROC>(loadProc("EGL_MakeCurrent"));
    EGL_QueryContext   = reinterpret_cast<PFNEGLQUERYCONTEXTPROC>(loadProc("EGL_QueryContext"));
    EGL_QueryString    = reinterpret_cast<PFNEGLQUERYSTRINGPROC>(loadProc("EGL_QueryString"));
    EGL_QuerySurface   = reinterpret_cast<PFNEGLQUERYSURFACEPROC>(loadProc("EGL_QuerySurface"));
    EGL_SwapBuffers    = reinterpret_cast<PFNEGLSWAPBUFFERSPROC>(loadProc("EGL_SwapBuffers"));
    EGL_Terminate      = reinterpret_cast<PFNEGLTERMINATEPROC>(loadProc("EGL_Terminate"));
    EGL_WaitGL         = reinterpret_cast<PFNEGLWAITGLPROC>(loadProc("EGL_WaitGL"));
    EGL_WaitNative     = reinterpret_cast<PFNEGLWAITNATIVEPROC>(loadProc("EGL_WaitNative"));
    EGL_BindTexImage   = reinterpret_cast<PFNEGLBINDTEXIMAGEPROC>(loadProc("EGL_BindTexImage"));
    EGL_ReleaseTexImage =
        reinterpret_cast<PFNEGLRELEASETEXIMAGEPROC>(loadProc("EGL_ReleaseTexImage"));
    EGL_SurfaceAttrib = reinterpret_cast<PFNEGLSURFACEATTRIBPROC>(loadProc("EGL_SurfaceAttrib"));
    EGL_SwapInterval  = reinterpret_cast<PFNEGLSWAPINTERVALPROC>(loadProc("EGL_SwapInterval"));
    EGL_BindAPI       = reinterpret_cast<PFNEGLBINDAPIPROC>(loadProc("EGL_BindAPI"));
    EGL_QueryAPI      = reinterpret_cast<PFNEGLQUERYAPIPROC>(loadProc("EGL_QueryAPI"));
    EGL_CreatePbufferFromClientBuffer = reinterpret_cast<PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC>(
        loadProc("EGL_CreatePbufferFromClientBuffer"));
    EGL_ReleaseThread = reinterpret_cast<PFNEGLRELEASETHREADPROC>(loadProc("EGL_ReleaseThread"));
    EGL_WaitClient    = reinterpret_cast<PFNEGLWAITCLIENTPROC>(loadProc("EGL_WaitClient"));
    EGL_GetCurrentContext =
        reinterpret_cast<PFNEGLGETCURRENTCONTEXTPROC>(loadProc("EGL_GetCurrentContext"));
    EGL_CreateSync     = reinterpret_cast<PFNEGLCREATESYNCPROC>(loadProc("EGL_CreateSync"));
    EGL_DestroySync    = reinterpret_cast<PFNEGLDESTROYSYNCPROC>(loadProc("EGL_DestroySync"));
    EGL_ClientWaitSync = reinterpret_cast<PFNEGLCLIENTWAITSYNCPROC>(loadProc("EGL_ClientWaitSync"));
    EGL_GetSyncAttrib  = reinterpret_cast<PFNEGLGETSYNCATTRIBPROC>(loadProc("EGL_GetSyncAttrib"));
    EGL_CreateImage    = reinterpret_cast<PFNEGLCREATEIMAGEPROC>(loadProc("EGL_CreateImage"));
    EGL_DestroyImage   = reinterpret_cast<PFNEGLDESTROYIMAGEPROC>(loadProc("EGL_DestroyImage"));
    EGL_GetPlatformDisplay =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYPROC>(loadProc("EGL_GetPlatformDisplay"));
    EGL_CreatePlatformWindowSurface = reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEPROC>(
        loadProc("EGL_CreatePlatformWindowSurface"));
    EGL_CreatePlatformPixmapSurface = reinterpret_cast<PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC>(
        loadProc("EGL_CreatePlatformPixmapSurface"));
    EGL_WaitSync                 = reinterpret_cast<PFNEGLWAITSYNCPROC>(loadProc("EGL_WaitSync"));
    EGL_SetBlobCacheFuncsANDROID = reinterpret_cast<PFNEGLSETBLOBCACHEFUNCSANDROIDPROC>(
        loadProc("EGL_SetBlobCacheFuncsANDROID"));
    EGL_GetCompositorTimingANDROID = reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGANDROIDPROC>(
        loadProc("EGL_GetCompositorTimingANDROID"));
    EGL_GetCompositorTimingSupportedANDROID =
        reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC>(
            loadProc("EGL_GetCompositorTimingSupportedANDROID"));
    EGL_GetFrameTimestampSupportedANDROID =
        reinterpret_cast<PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC>(
            loadProc("EGL_GetFrameTimestampSupportedANDROID"));
    EGL_GetFrameTimestampsANDROID = reinterpret_cast<PFNEGLGETFRAMETIMESTAMPSANDROIDPROC>(
        loadProc("EGL_GetFrameTimestampsANDROID"));
    EGL_GetNextFrameIdANDROID =
        reinterpret_cast<PFNEGLGETNEXTFRAMEIDANDROIDPROC>(loadProc("EGL_GetNextFrameIdANDROID"));
    EGL_GetNativeClientBufferANDROID = reinterpret_cast<PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC>(
        loadProc("EGL_GetNativeClientBufferANDROID"));
    EGL_DupNativeFenceFDANDROID = reinterpret_cast<PFNEGLDUPNATIVEFENCEFDANDROIDPROC>(
        loadProc("EGL_DupNativeFenceFDANDROID"));
    EGL_PresentationTimeANDROID = reinterpret_cast<PFNEGLPRESENTATIONTIMEANDROIDPROC>(
        loadProc("EGL_PresentationTimeANDROID"));
    EGL_CreateDeviceANGLE =
        reinterpret_cast<PFNEGLCREATEDEVICEANGLEPROC>(loadProc("EGL_CreateDeviceANGLE"));
    EGL_ReleaseDeviceANGLE =
        reinterpret_cast<PFNEGLRELEASEDEVICEANGLEPROC>(loadProc("EGL_ReleaseDeviceANGLE"));
    EGL_QueryDisplayAttribANGLE = reinterpret_cast<PFNEGLQUERYDISPLAYATTRIBANGLEPROC>(
        loadProc("EGL_QueryDisplayAttribANGLE"));
    EGL_QueryStringiANGLE =
        reinterpret_cast<PFNEGLQUERYSTRINGIANGLEPROC>(loadProc("EGL_QueryStringiANGLE"));
    EGL_ProgramCacheGetAttribANGLE = reinterpret_cast<PFNEGLPROGRAMCACHEGETATTRIBANGLEPROC>(
        loadProc("EGL_ProgramCacheGetAttribANGLE"));
    EGL_ProgramCachePopulateANGLE = reinterpret_cast<PFNEGLPROGRAMCACHEPOPULATEANGLEPROC>(
        loadProc("EGL_ProgramCachePopulateANGLE"));
    EGL_ProgramCacheQueryANGLE =
        reinterpret_cast<PFNEGLPROGRAMCACHEQUERYANGLEPROC>(loadProc("EGL_ProgramCacheQueryANGLE"));
    EGL_ProgramCacheResizeANGLE = reinterpret_cast<PFNEGLPROGRAMCACHERESIZEANGLEPROC>(
        loadProc("EGL_ProgramCacheResizeANGLE"));
    EGL_QuerySurfacePointerANGLE = reinterpret_cast<PFNEGLQUERYSURFACEPOINTERANGLEPROC>(
        loadProc("EGL_QuerySurfacePointerANGLE"));
    EGL_CreateStreamProducerD3DTextureANGLE =
        reinterpret_cast<PFNEGLCREATESTREAMPRODUCERD3DTEXTUREANGLEPROC>(
            loadProc("EGL_CreateStreamProducerD3DTextureANGLE"));
    EGL_StreamPostD3DTextureANGLE = reinterpret_cast<PFNEGLSTREAMPOSTD3DTEXTUREANGLEPROC>(
        loadProc("EGL_StreamPostD3DTextureANGLE"));
    EGL_GetSyncValuesCHROMIUM =
        reinterpret_cast<PFNEGLGETSYNCVALUESCHROMIUMPROC>(loadProc("EGL_GetSyncValuesCHROMIUM"));
    EGL_QueryDeviceAttribEXT =
        reinterpret_cast<PFNEGLQUERYDEVICEATTRIBEXTPROC>(loadProc("EGL_QueryDeviceAttribEXT"));
    EGL_QueryDeviceStringEXT =
        reinterpret_cast<PFNEGLQUERYDEVICESTRINGEXTPROC>(loadProc("EGL_QueryDeviceStringEXT"));
    EGL_QueryDisplayAttribEXT =
        reinterpret_cast<PFNEGLQUERYDISPLAYATTRIBEXTPROC>(loadProc("EGL_QueryDisplayAttribEXT"));
    EGL_CreatePlatformPixmapSurfaceEXT = reinterpret_cast<PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC>(
        loadProc("EGL_CreatePlatformPixmapSurfaceEXT"));
    EGL_CreatePlatformWindowSurfaceEXT = reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC>(
        loadProc("EGL_CreatePlatformWindowSurfaceEXT"));
    EGL_GetPlatformDisplayEXT =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(loadProc("EGL_GetPlatformDisplayEXT"));
    EGL_DebugMessageControlKHR =
        reinterpret_cast<PFNEGLDEBUGMESSAGECONTROLKHRPROC>(loadProc("EGL_DebugMessageControlKHR"));
    EGL_LabelObjectKHR = reinterpret_cast<PFNEGLLABELOBJECTKHRPROC>(loadProc("EGL_LabelObjectKHR"));
    EGL_QueryDebugKHR  = reinterpret_cast<PFNEGLQUERYDEBUGKHRPROC>(loadProc("EGL_QueryDebugKHR"));
    EGL_ClientWaitSyncKHR =
        reinterpret_cast<PFNEGLCLIENTWAITSYNCKHRPROC>(loadProc("EGL_ClientWaitSyncKHR"));
    EGL_CreateSyncKHR  = reinterpret_cast<PFNEGLCREATESYNCKHRPROC>(loadProc("EGL_CreateSyncKHR"));
    EGL_DestroySyncKHR = reinterpret_cast<PFNEGLDESTROYSYNCKHRPROC>(loadProc("EGL_DestroySyncKHR"));
    EGL_GetSyncAttribKHR =
        reinterpret_cast<PFNEGLGETSYNCATTRIBKHRPROC>(loadProc("EGL_GetSyncAttribKHR"));
    EGL_CreateImageKHR = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(loadProc("EGL_CreateImageKHR"));
    EGL_DestroyImageKHR =
        reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(loadProc("EGL_DestroyImageKHR"));
    EGL_CreateStreamKHR =
        reinterpret_cast<PFNEGLCREATESTREAMKHRPROC>(loadProc("EGL_CreateStreamKHR"));
    EGL_DestroyStreamKHR =
        reinterpret_cast<PFNEGLDESTROYSTREAMKHRPROC>(loadProc("EGL_DestroyStreamKHR"));
    EGL_QueryStreamKHR = reinterpret_cast<PFNEGLQUERYSTREAMKHRPROC>(loadProc("EGL_QueryStreamKHR"));
    EGL_QueryStreamu64KHR =
        reinterpret_cast<PFNEGLQUERYSTREAMU64KHRPROC>(loadProc("EGL_QueryStreamu64KHR"));
    EGL_StreamAttribKHR =
        reinterpret_cast<PFNEGLSTREAMATTRIBKHRPROC>(loadProc("EGL_StreamAttribKHR"));
    EGL_StreamConsumerAcquireKHR = reinterpret_cast<PFNEGLSTREAMCONSUMERACQUIREKHRPROC>(
        loadProc("EGL_StreamConsumerAcquireKHR"));
    EGL_StreamConsumerGLTextureExternalKHR =
        reinterpret_cast<PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC>(
            loadProc("EGL_StreamConsumerGLTextureExternalKHR"));
    EGL_StreamConsumerReleaseKHR = reinterpret_cast<PFNEGLSTREAMCONSUMERRELEASEKHRPROC>(
        loadProc("EGL_StreamConsumerReleaseKHR"));
    EGL_SwapBuffersWithDamageKHR = reinterpret_cast<PFNEGLSWAPBUFFERSWITHDAMAGEKHRPROC>(
        loadProc("EGL_SwapBuffersWithDamageKHR"));
    EGL_WaitSyncKHR = reinterpret_cast<PFNEGLWAITSYNCKHRPROC>(loadProc("EGL_WaitSyncKHR"));
    EGL_PostSubBufferNV =
        reinterpret_cast<PFNEGLPOSTSUBBUFFERNVPROC>(loadProc("EGL_PostSubBufferNV"));
    EGL_StreamConsumerGLTextureExternalAttribsNV =
        reinterpret_cast<PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALATTRIBSNVPROC>(
            loadProc("EGL_StreamConsumerGLTextureExternalAttribsNV"));
}
}  // namespace angle
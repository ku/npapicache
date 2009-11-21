/* ***** BEGIN LICENSE BLOCK *****
 * (C)opyright 2008 Aplix Corporation. anselm@aplixcorp.com
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * ***** END LICENSE BLOCK ***** */

 #include <shlobj.h> 

#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
  #include <time.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

typedef unsigned short uint16;
typedef int int32;
typedef short int16;

#include "./npapi.h"
#include "./nptypes.h"
#include "./npruntime.h"
#include "./npfunctions.h"


NPP __instance;
#if 0

#if defined(XULRUNNER_SDK)
#include <npapi.h>
#include <npupp.h>
#include <npruntime.h>
#elif defined(WEBKIT_DARWIN_SDK)
#include <Webkit/npapi.h>
#include <WebKit/npfunctions.h>
#include <WebKit/npruntime.h>
#define OSCALL
#elif defined(WEBKIT_WINMOBILE_SDK) /* WebKit SDK on Windows */
#ifndef PLATFORM
#define PLATFORM(x) defined(x)
#endif
#include <npfunctions.h>
#ifndef OSCALL
#define OSCALL WINAPI
#endif
#endif

#endif

static NPObject *so              = NULL;
static NPNetscapeFuncs *npnfuncs = NULL;

std::map<std::string, int> _requests;


/* NPN */

static void logmsg(const char *msg) {
	FILE *out = fopen("c:\\npsimple.log", "ab");
	fputs(msg, out);
	fclose(out);
}

static bool
hasMethod(NPObject* obj, NPIdentifier methodName) {
	char *name = npnfuncs->utf8fromidentifier(methodName);
	if ( strcmp(name, "enqueue") == 0 ) {
		return true;
	} else if ( strcmp(name, "isFetching") == 0 ) {
		return true;
	} else {
		return false;
	}
}

static bool
invokeDefault(NPObject *obj, const NPVariant *args, uint32_t argCount, NPVariant *result) {
	return false;
}

static bool
invoke(NPObject* obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result) {
	NPUTF8 *name = npnfuncs->utf8fromidentifier(methodName);

		   NPString arg0 = NPVARIANT_TO_STRING(args[0]);
	const	char* url = arg0.UTF8Characters;


	  IMalloc *pMalloc;
    SHGetMalloc( &pMalloc );

    TCHAR szPath[_MAX_PATH];
    LPITEMIDLIST pidl;
	HWND hWnd = NULL; 
    if( SUCCEEDED(SHGetSpecialFolderLocation(hWnd,CSIDL_APPDATA     ,&pidl)) )
    { 
        SHGetPathFromIDList(pidl,szPath); // ƒpƒX‚É•ÏŠ·‚·‚é
        pMalloc->Free(pidl);              // Žæ“¾‚µ‚½IDL‚ð‰ð•ú‚·‚é (CoTaskMemFree‚Å‚à‰Â)
    }

    pMalloc->Release();

char pszChar[1024];
int	nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,szPath,-1,NULL,0,NULL,NULL);
		
		if(pszChar)
		{
			//•ÏŠ·
			nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,szPath,(int)::wcslen(szPath)+1,pszChar,nLen,NULL,NULL);
			if(nLen == 0)
			{
				// = NULL;
			}
		}

logmsg("path");


logmsg((char*)pszChar);
	if(name && !strcmp((const char *)name, "enqueue")) {
	//	strcat(pszChar, "\\Chromium\\User Data\\Default\\Extensions\\cdoinchhmmllfnfecinbngpgokdmlpbf\\0.2.0");
//		strcat((char*)szPath, "\\cache");
		char* name = _tempnam("\\cache","html5");
		logmsg(name);
		logmsg("\n");

	FILE *out = fopen(name, "wb");
	npnfuncs->geturlnotify(__instance, url, 
		NULL, out);



//		BOOLEAN_TO_NPVARIANT(true, *result);
		char* p=	(char*)npnfuncs->memalloc((uint32_t)strlen(name));
		strcpy(p, name);

		STRINGZ_TO_NPVARIANT(p, *result);
		
		int n = 1;
		_requests.insert( std::make_pair<std::string, int >( std::string(url), 1 ));



//			npnfuncs->geturl(
//		NPN_GetURL(, NULL);
		return true;
		//return invokeDefault(obj, args, argCount, result);
	} else if ( strcmp(name, "isFetching") == 0 ) {
		NPString arg0 = NPVARIANT_TO_STRING(args[0]);

		std::map<std::string, int>::iterator it;
		it = _requests.find( std::string(url) );
		if ( it != _requests.end() ) {
		BOOLEAN_TO_NPVARIANT(true, *result);
	} else {
			BOOLEAN_TO_NPVARIANT(false, *result);
	}
		return true;
}	else {
		// aim exception handling
		npnfuncs->setexception(obj, "exception during invocation");
		return false;
	}
}

static bool
hasProperty(NPObject *obj, NPIdentifier propertyName) {
	return false;
}

static bool
getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result) {
	return false;
}

static NPClass npcRefObject = {
	NP_CLASS_STRUCT_VERSION,
	NULL,
	NULL,
	NULL,
	hasMethod,
	invoke,
	invokeDefault,
	hasProperty,
	getProperty,
	NULL,
	NULL,
};

/* NPP */

static NPError
nevv(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, char *argn[], char *argv[], NPSavedData *saved) {
	return NPERR_NO_ERROR;
}

static NPError
destroy(NPP instance, NPSavedData **save) {
	if(so)
		npnfuncs->releaseobject(so);
	so = NULL;
	return NPERR_NO_ERROR;
}

static NPError
getValue(NPP instance, NPPVariable variable, void *value) {
	switch(variable) {
	default:
		return NPERR_GENERIC_ERROR;
	case NPPVpluginNameString:
		*((char **)value) = "HelooWorldPlugin";
		break;
	case NPPVpluginDescriptionString:
		*((char **)value) = "chrome extension hackathon html5 group plugin.";
		break;
	case NPPVpluginScriptableNPObject:
		if(!so)
			so = npnfuncs->createobject(instance, &npcRefObject);
		npnfuncs->retainobject(so);
		*(NPObject **)value = so;
		break;
#if defined(XULRUNNER_SDK)
	case NPPVpluginNeedsXEmbed:
//		*((PRBool *)value) = PR_FALSE;
		break;
#endif
	}
	return NPERR_NO_ERROR;
}

static NPError /* expected by Safari on Darwin */
handleEvent(NPP instance, void *ev) {
	logmsg("npsimple: handleEvent\n");
	return NPERR_NO_ERROR;
}

static NPError /* expected by Opera */
setWindow(NPP instance, NPWindow* pNPWindow) {
	logmsg("npsimple: setWindow\n");

	__instance = instance;
	return NPERR_NO_ERROR;
}

/* EXPORT */
#ifdef __cplusplus
extern "C" {
#endif

	
void
NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
		logmsg("notfi\n");
	if (reason ==  NPRES_BASE )
		logmsg("base\n");
if ( reason == NPRES_DONE )
		logmsg("done\n");
if ( reason == NPRES_NETWORK_ERR )
		logmsg("neerr\n");
if ( reason == NPRES_USER_BREAK )
		logmsg("abort\n");
}

NPError
NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
	logmsg(__FUNCTION__"\n");
	logmsg(type);
	logmsg("\n");
//	*stype = NP_ASFILEONLY;
	*stype = NP_NORMAL;
//	char fname [128];

//	stream->notifyData = out;


	return NPERR_NO_ERROR;
}


int32_t
NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
	FILE* out = (FILE*)stream->notifyData;
	size_t n =	fwrite(buffer, 1, len, out);
  return (int32_t)n;
}
void
NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
  logmsg(__FUNCTION__"\n");
  logmsg(fname);
  logmsg("\n");
}
NPError
NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
FILE* out = (FILE*)stream->notifyData;
fclose(out);
 logmsg(__FUNCTION__"\n");
  logmsg(stream->url);
  logmsg("\n");
	_requests.erase( std::string(stream->url) );

//	NPVariant prop_value;
	NPIdentifier methodName = npnfuncs->getstringidentifier("toString");

  //  NPVariant strv;
//STRINGZ_TO_NPVARIANT("div", strv);
/*
	npnfuncs->invoke (instance, so,  methodName,
                     &strv, 1, &prop_value);
*/

return NPERR_NO_ERROR;
}

int32_t
NPP_WriteReady(NPP instance, NPStream* stream)
{
	return 2048;
}

NPError OSCALL
NP_GetEntryPoints(NPPluginFuncs *nppfuncs) {
	logmsg("npsimple: NP_GetEntryPoints\n");
	nppfuncs->version       = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
	nppfuncs->newp          = nevv;
	nppfuncs->destroy       = destroy;
	nppfuncs->getvalue      = getValue;
	nppfuncs->event         = handleEvent;
	nppfuncs->setwindow     = setWindow;
	nppfuncs->urlnotify = NPP_URLNotify;

	nppfuncs->newstream = NPP_NewStream;
	 nppfuncs->writeready = NPP_WriteReady;
	nppfuncs->write = NPP_Write;
	nppfuncs->destroystream = NPP_DestroyStream;
	nppfuncs->asfile = NPP_StreamAsFile;

	return NPERR_NO_ERROR;
}

#ifndef HIBYTE
#define HIBYTE(x) ((((uint32)(x)) & 0xff00) >> 8)
#endif

NPError OSCALL
NP_Initialize(NPNetscapeFuncs *npnf
#if !defined(_WINDOWS) && !defined(WEBKIT_DARWIN_SDK)
			, NPPluginFuncs *nppfuncs)
#else
			)
#endif
{
	logmsg("npsimple: NP_Initialize\n");
	if(npnf == NULL)
		return NPERR_INVALID_FUNCTABLE_ERROR;

	if(HIBYTE(npnf->version) > NP_VERSION_MAJOR)
		return NPERR_INCOMPATIBLE_VERSION_ERROR;

	npnfuncs = npnf;
#if !defined(_WINDOWS) && !defined(WEBKIT_DARWIN_SDK)
	NP_GetEntryPoints(nppfuncs);
#endif
	return NPERR_NO_ERROR;
}

NPError
OSCALL NP_Shutdown() {
	logmsg("npsimple: NP_Shutdown\n");
	return NPERR_NO_ERROR;
}

char *
NP_GetMIMEDescription(void) {
	logmsg("npsimple: NP_GetMIMEDescription\n");
	return "application/x-vnd-aplix-foo:.foo:anselm@aplix.co.jp";
}

NPError OSCALL /* needs to be present for WebKit based browsers */
NP_GetValue(void *npp, NPPVariable variable, void *value) {
	return getValue((NPP)npp, variable, value);
}

#ifdef __cplusplus
}
#endif

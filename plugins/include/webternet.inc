/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod (C)2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This file is part of the SourceMod/SourcePawn SDK.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#if defined _webternet_included
  #endinput
#endif
#define _webternet_included

/**
 * @section		HTTP request natives.
 */

/**
 * HTTP download function callbacks.
 */
funcenum HTTP_SessionCallback {
	// Callback with session handle and request result
	public(Handle:session, bool:succeeded),
	// Callback with session handle, request result and download handle
	public(Handle:session, bool:succeeded, Handle:downloader),
	// Callback with session handle, request result, download handle and optional user data
	public(Handle:session, bool:succeeded, Handle:downloader, any:data),
};

methodmap HTTPDownloader < Handle {
	// Returns the response body content from a download handle.
	//
	// @param body			The buffer to set to copy the body to.
	// @param maxlen		The maximum string length of the buffer.
	// @return				True on success, false on fail/error.
	//
	// @note				Use this function to receive the response body as an UTF-8 string.
	// 						Size information can be obtained by using HTTP_GetBodySize()
	//						Beware that this function fails if the buffer is too small;
	//						count in one extra byte for the NULL-terminator.
	public native bool GetBodyContent(char[] body, int maxlen);

	// Returns the size of a finished request response in bytes.
	//
	// @return				The size of the response in bytes, 0 otherwise.
	//
	// @note				Use this function to allocate a string within SP to hold in-memory
	//						response data. Do not forget to add one extra byte for the termination.
	//						See HTTP_GetBodyContent() for more information.
	property int BodySize {
		public native get();
	}
}

methodmap HTTPFileDownloader < HTTPDownloader {
	// Creates a new file-based download handler. It stores the response as a local file.
	//
	// @param file			The path to the local file the response shall be stored in.
	//
	// @note				Use this function if you want to download (large) binary content
	//						to disk or e.g. save a Kv-formatted response to a local text file.
	public native HTTPFileDownloader(const char[] file);
}

methodmap HTTPMemoryDownloader < HTTPDownloader {
	// Creates a new memory-based download handler. It stores the response in memory.
	//
	// @note				Use this function if you want to directly access a (text) response
	//						from within SP as an UTF-8 encoded string.
	//						For more information see HTTP_GetBodySize() and HTTP_GetBodyContent()
	public native HTTPMemoryDownloader();
}

methodmap HTTPSession < Handle {
	// Creates a new ready-to-use HTTP session handle.
	//
	// @note				You can only perform one request (GET, POST, ...) per session handle.
	// 						For multiple sequential asynchronous requests (e.g. in a loop) you have
	//						to create a new session and free it with CloseHandle()
	public native HTTPSession();

	// Sets whether an HTTP failure (>= 400) returns false from the request functions.
	//
	// @param fail			True to fail, false otherwise.
	// @return				True on success, false on fail/error.
	//
	// @note				The default behaviour is to treat HTTP 4xx errors as an acceptable
	// 						condition. The corresponding error page generated by the web server
	//						is returned. If set to true, request functions report a failure.
	public native bool SetFailOnHTTPError(bool fail);

	// Returns the last error happened on a session as human readable message.
	//
	// @param error			The target buffer to hold the error message.
	// @param maxlen		The maximum string length of the buffer.
	//
	// @return				True on success, false on fail/error.
	public native bool GetLastError(char[] error, int maxlen);

	// Initiates an asynchronous download attempt (GET-request) for a given URL.
	//
	// @param downloader	The download handle this download should use.
	// @param url			The target URL.
	// @param callback		The callback function to be executed on request completion.
	// @param data			Optional custom user data.
	// @return				True on success, false on fail/error.
	//
	// @note				While the download action is in progress, the session handle is
	//						invalid and can't be accessed. For multiple parallel requests
	//						create a new session with HTTP_CreateSession() for every request.
	//						Session and download handle have to be freed with CloseHandle()
	//						after the corresponding callback function has been executed.
	public native bool Download(HTTPDownloader downloader, const char[] url, HTTP_SessionCallback callback, any data = 0);

	// Initiates an asynchronous download attempt (POST-request) for a given URL.
	//
	// @param downloader	The download handle this download should use.
	// @param downloader	The web form handle this download should use.
	// @param url			The target URL.
	// @param callback		The callback function to be executed on request completion.
	// @param data			Optional custom user data.
	// @return				True on success, false on fail/error.
	//
	// @note				While the download action is in progress, the session handle is
	//						invalid and can't be accessed. For multiple parallel requests
	//						create a new session with HTTP_CreateSession() for every request.
	//						Session and download handle have to be freed with CloseHandle()
	//						after the corresponding callback function has been executed.
	//						The web form handle is not closed as it may be reused.
	public native bool PostAndDownload(HTTPDownloader downloader, HTTPWebForm form, const char[] url, HTTP_SessionCallback callback, any data = 0);
}

methodmap HTTPWebForm < Handle {
	// Creates a new empty web form handle.
	//
	// @note				This function may be used with HTTP_PostAndDownload() to simulate
	//						an application/x-www-form-urlencoded form POST request.
	//						Also see HTTP_AddStringToWebForm() and HTTP_AddFileToWebForm()
	public native HTTPWebForm();

	// Adds a text key-value-pair (e.g. game=hl2) to an existing web form.
	//
	// @param name			The name section of the pair (e.g. game)
	// @param data			The value section of the pair (e.g. hl2)
	// @return				True on success, false on fail/error.
	//
	// @note				The supplied data gets URL-encoded automatically.
	public native bool AddString(const char[] name, const char[] data);

	// Adds a local file to upload to an existing web form.
	//
	// @param name			The field name.
	// @param path			The local file path.
	// @return				True on success, false on fail/error.
	public native bool AddFile(const char[] name, const char[] path);
}

/**
 * Creates a new ready-to-use HTTP session handle. 
 * 
 * @return				Valid session handle on success, INVALID_HANDLE on failure.
 * 
 * @note				You can only perform one request (GET, POST, ...) per session handle.
 * 						For multiple sequential asynchronous requests (e.g. in a loop) you have
 *						to create a new session and free it with CloseHandle()
 */
native Handle:HTTP_CreateSession();

/**
 * Sets whether an HTTP failure (>= 400) returns false from the request functions.
 * 
 * @param session		HTTP session handle from HTTP_CreateSession()
 * @param fail			True to fail, false otherwise.
 * @return				True on success, false on fail/error.
 * 
 * @note				The default behaviour is to treat HTTP 4xx errors as an acceptable
 * 						condition. The corresponding error page generated by the web server
 *						is returned. If set to true, request functions report a failure.
 */
native bool:HTTP_SetFailOnHTTPError(Handle:session, bool:fail);

/**
 * Returns the last error happened on a session as human readable message.
 * 
 * @param session		The HTTP session handle to retrieve the error from.
 * @param error			The target buffer to hold the error message.
 * @param maxlen		The maximum string length of the buffer.
 * 
 * @return				True on success, false on fail/error.
 */
native bool:HTTP_GetLastError(Handle:session, String:error[], maxlen);

/**
 * Creates a new file-based download handler. It stores the response as a local file.
 * 
 * @param file			The path to the local file the response shall be stored in.
 * @return				Valid download handle on success, INVALID_HANDLE on failure.
 * 
 * @note				Use this function if you want to download (large) binary content
 *						to disk or e.g. save a Kv-formatted response to a local text file.
 */
native Handle:HTTP_CreateFileDownloader(const String:file[]);

/**
 * Creates a new memory-based download handler. It stores the response in memory.
 * 
 * @return				Valid download handle on success, INVALID_HANDLE on failure.
 * 
 * @note				Use this function if you want to directly access a (text) response
 *						from within SP as an UTF-8 encoded string.
 *						For more information see HTTP_GetBodySize() and HTTP_GetBodyContent()
 */
native Handle:HTTP_CreateMemoryDownloader();

/**
 * Returns the size of a finished request response in bytes.
 * 
 * @param downloader	The download handle to receive size information from.
 * @return				The size of the response in bytes, 0 otherwise.
 * 
 * @note				Use this function to allocate a string within SP to hold in-memory
 *						response data. Do not forget to add one extra byte for the termination.
 *						See HTTP_GetBodyContent() for more information.
 */
native HTTP_GetBodySize(Handle:downloader);

/**
 * Returns the response body content from a download handle.
 * 
 * @param downloader	The download handle to receive the body content from.
 * @param body			The buffer to set to copy the body to.
 * @param maxlen		The maximum string length of the buffer.
 * @return				True on success, false on fail/error.
 * 
 * @note				Use this function to receive the response body as an UTF-8 string.
 * 						Size information can be obtained by using HTTP_GetBodySize()
 *						Beware that this function fails if the buffer is too small;
 *						count in one extra byte for the NULL-terminator.
 */
native bool:HTTP_GetBodyContent(Handle:downloader, String:body[], maxlen);

/**
 * Creates a new empty web form handle.
 * 
 * @return				Valid web form handle on success, INVALID_HANDLE on failure.
 * 
 * @note				This function may be used with HTTP_PostAndDownload() to simulate
 *						an application/x-www-form-urlencoded form POST request.
 *						Also see HTTP_AddStringToWebForm() and HTTP_AddFileToWebForm()
 */
native Handle:HTTP_CreateWebForm();

/**
 * Adds a text key-value-pair (e.g. game=hl2) to an existing web form.
 * 
 * @param form			The web form handle the pair should be added to.
 * @param name			The name section of the pair (e.g. game)
 * @param data			The value section of the pair (e.g. hl2)
 * @return				True on success, false on fail/error.
 * 
 * @note				The supplied data gets URL-encoded automatically.
 */
native bool:HTTP_AddStringToWebForm(Handle:form, const String:name[], const String:data[]);

/**
 * Adds a local file to upload to an existing web form.
 * 
 * @param form			The web form handle the file should be added to.
 * @param name			The field name.
 * @param path			The local file path.
 * @return				True on success, false on fail/error.
 */
native bool:HTTP_AddFileToWebForm(Handle:form, const String:name[], const String:path[]);

/**
 * Initiates an asynchronous download attempt (GET-request) for a given URL.
 * 
 * @param session		The session handle this download should use.
 * @param downloader	The download handle this download should use.
 * @param url			The target URL.
 * @param callback		The callback function to be executed on request completion.
 * @param data			Optional custom user data.
 * @return				True on success, false on fail/error.
 * 
 * @note				While the download action is in progress, the session handle is
 *						invalid and can't be accessed. For multiple parallel requests
 *						create a new session with HTTP_CreateSession() for every request.
 *						Session and download handle have to be freed with CloseHandle()
 *						after the corresponding callback function has been executed.
 */
native bool:HTTP_Download(Handle:session, Handle:downloader, const String:url[], HTTP_SessionCallback:callback, any:data = INVALID_HANDLE);

/**
 * Initiates an asynchronous download attempt (POST-request) for a given URL.
 * 
 * @param session		The session handle this download should use.
 * @param downloader	The download handle this download should use.
 * @param downloader	The web form handle this download should use.
 * @param url			The target URL.
 * @param callback		The callback function to be executed on request completion.
 * @param data			Optional custom user data.
 * @return				True on success, false on fail/error.
 * 
 * @note				While the download action is in progress, the session handle is
 *						invalid and can't be accessed. For multiple parallel requests
 *						create a new session with HTTP_CreateSession() for every request.
 *						Session and download handle have to be freed with CloseHandle()
 *						after the corresponding callback function has been executed.
 *						The web form handle is not closed as it may be reused.
 */
native bool:HTTP_PostAndDownload(Handle:session, Handle:downloader, Handle:form, const String:url[], HTTP_SessionCallback:callback, any:data = INVALID_HANDLE);

/**
 * @endsection
 */

/**
 * Do not edit below this line!
 */
public Extension:__ext_webternet = 
{
	name = "webternet",
	file = "webternet.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
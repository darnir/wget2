/*
 * Copyright(c) 2017 Free Software Foundation, Inc.
 *
 * This file is part of libwget.
 *
 * Libwget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Libwget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libwget.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Test special UTF-8 char in locale part of subdirectory given as command line argument.
 * A ~ (tilde) in SHIFT-JIS becomes translated into ‾ (%E2%80%BE) in UTF-8 and vice versa.
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h> // exit()
#include "libtest.h"

int main(void)
{
	wget_test_url_t urls[]={
		{	.name = "/~foo/index.html",
			.code = "200 Dontcare",
			.body =
				"<html><head>"
				"  <meta http-equiv=\"Content-Type\" content=\"text/html;charset=Shift_JIS\">"
				"</head><body>"
				"  <a href=\"bar.html\">"
				"  <a href=\"~baz.html\">"
				"</body></html>",
			.headers = {
				"Content-Type: text/html",
			}
		},
		{	.name = "/~foo/bar.html", // UTF-8 encoded
			.code = "200 Dontcare",
			.body =
				"<html><head><title>bar</title><p>hello</p></body></html>",
		},
		{	.name = "/~foo/%E2%80%BEbaz.html", // UTF-8 + percent encoded (IRI standard)
			.code = "200 Dontcare",
			.body =
				"<html><head><title>baz</title><p>hello</p></body></html>",
		},
	};

	// functions won't come back if an error occurs
	wget_test_start_server(
		WGET_TEST_RESPONSE_URLS, &urls, countof(urls),
		0);

	// test-iri-disabled
	wget_test(
//		WGET_TEST_KEEP_TMPFILES, 1,
		WGET_TEST_OPTIONS, "--no-robots -nH -r --local-encoding=UTF-8",
		WGET_TEST_REQUEST_URL, "~foo/",
		WGET_TEST_EXPECTED_ERROR_CODE, 0,
		WGET_TEST_EXPECTED_FILES, &(wget_test_file_t []) {
			{ urls[0].name + 1, urls[0].body }, // ~foo/index.html
			{ urls[1].name + 1, urls[1].body }, // ~foo/bar.html
			{ "~foo/‾baz.html", urls[2].body }, // unescaped + UTF-8, matching local-encoding
			{	NULL } },
		0);

	exit(0);
}

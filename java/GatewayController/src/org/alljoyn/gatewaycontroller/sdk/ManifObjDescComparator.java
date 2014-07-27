/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

package org.alljoyn.gatewaycontroller.sdk;

import java.util.Comparator;

import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppObjectPath;

/**
 * Allows to compare {@link ManifestObjectDescription} according to there
 * {@link ConnAppObjectPath}. The algorithm performs lexicographical comparison
 * of the object paths with the condition that for equal object paths the object
 * path that is not defined as prefix is less than the object path that is
 * prefix.
 */
class ManifObjDescComparator implements Comparator<ManifestObjectDescription> {

    /**
     * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
     */
    @Override
    public int compare(ManifestObjectDescription lhs, ManifestObjectDescription rhs) {

        String lhsOP = lhs.getObjectPath().getPath();
        String rhsOP = rhs.getObjectPath().getPath();

        int compRes = rhsOP.compareTo(lhsOP);
        if (compRes == 0) {

            if (lhs.getObjectPath().isPrefix()) {
                return 1;
            } else {
                return -1;
            }
        }

        return compRes;
    }

}

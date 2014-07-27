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

import org.alljoyn.gatewaycontroller.sdk.AccessControlList.AclResponseCode;

/**
 * The class holds the {@link AclResponseCode} returned by the invocation of the
 * createAcl or updateAcl methods. In addition it holds the {@link AccessRules}
 * object with the rules that do not comply with the {@link ManifestRules}
 */
public class AclWriteResponse {

    /**
     * Acl id
     */
    private final String id;

    /**
     * Response code
     */
    private final AclResponseCode code;

    /**
     * ACL object path
     */
    private final String objectPath;

    /**
     * The rules that don't comply with the {@link ManifestRules}
     */
    private final AccessRules invalidRules;

    /**
     * Constructor
     * 
     * @param id
     *            ACL id
     * @param code
     *            {@link AclResponseCode}
     * @param invalidRules
     *            {@link AccessRules}
     * @param objPath
     *            Object path
     */
    AclWriteResponse(String id, AclResponseCode code, AccessRules invalidRules, String objPath) {

        this.id             = id;
        this.code           = code;
        this.invalidRules   = invalidRules;
        this.objectPath     = objPath;
    }

    /**
     * @return The id of the ACL that the write operation was referred to
     */
    public String getAclId() {
        return id;
    }

    /**
     * @return {@link AclResponseCode} of the ACL write action
     */
    public AclResponseCode getResponseCode() {
        return code;
    }

    /**
     * @return {@link AccessRules} with the rules that don't comply with the {@link ManifestRules}
     */
    public AccessRules getInvalidRules() {
        return invalidRules;
    }

    /**
     * @return {@link AccessControlList} object path
     */
    public String getObjectPath() {
        return objectPath;
    }
}

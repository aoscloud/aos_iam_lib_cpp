/*
 * Copyright (C) 2023 Renesas Electronics Corporation.
 * Copyright (C) 2023 EPAM Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AOS_IAM_CONFIG_HPP_
#define AOS_IAM_CONFIG_HPP_

/**
 * Max length of certificate type.
 */
#ifndef AOS_CONFIG_CERTHANDLER_CERT_TYPE_NAME_LEN
#define AOS_CONFIG_CERTHANDLER_CERT_TYPE_NAME_LEN 16
#endif

/**
 * Max number of key usages for a module.
 */
#ifndef AOS_CONFIG_CERTHANDLER_KEY_USAGE_MAX_COUNT
#define AOS_CONFIG_CERTHANDLER_KEY_USAGE_MAX_COUNT 2
#endif

/**
 * Max number of certificate modules.
 */
#ifndef AOS_CONFIG_CERTHANDLER_MODULES_MAX_COUNT
#define AOS_CONFIG_CERTHANDLER_MODULES_MAX_COUNT 10
#endif

/**
 * Max expected number of certificates in a chain stored in PEM file.
 */
#ifndef AOS_CONFIG_CERTHANDLER_CERTS_CHAIN_SIZE
#define AOS_CONFIG_CERTHANDLER_CERTS_CHAIN_SIZE 3
#endif

/**
 * Max expected number of certificates per IAM certificate module.
 */
#ifndef AOS_CONFIG_CERTHANDLER_MAX_CERTS_PER_MODULE
#define AOS_CONFIG_CERTHANDLER_MAX_CERTS_PER_MODULE 5
#endif

/**
 * Password length.
 */
#ifndef AOS_CONFIG_CERTHANDLER_PASSWORD_LEN
#define AOS_CONFIG_CERTHANDLER_PASSWORD_LEN 40
#endif

/**
 * Maximum length of distinguished name string representation.
 */
#ifndef AOS_CONFIG_CERTHANDLER_DN_STRING_LEN
#define AOS_CONFIG_CERTHANDLER_DN_STRING_LEN 128
#endif

#endif
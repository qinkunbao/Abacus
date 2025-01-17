/*
 * Copyright 2002-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 *
 * Portions of the attached software ("Contribution") are developed by
 * SUN MICROSYSTEMS, INC., and are contributed to the OpenSSL project.
 *
 * The Contribution is licensed pursuant to the OpenSSL open source
 * license provided above.
 *
 * The elliptic curve binary polynomial software is originally written by
 * Sheueling Chang Shantz and Douglas Stebila of Sun Microsystems Laboratories.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/opensslconf.h> /* To see if OPENSSL_NO_EC is defined */

#ifdef OPENSSL_NO_EC
int main(int argc, char *argv[])
{
    puts("Elliptic curves are disabled.");
    return 0;
}
#else

#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif
#include <openssl/err.h>
#include <openssl/rand.h>

static const char rnd_seed[] = "string to make the random number generator "
                               "think it has entropy";

/* declaration of the test functions */
int x9_62_tests(BIO *);
int x9_62_test_internal(BIO *out, int nid, const char *r, const char *s);
int test_builtin(BIO *);

/* functions to change the RAND_METHOD */
int change_rand(void);
int restore_rand(void);
int fbytes(unsigned char *buf, int num);

static RAND_METHOD fake_rand;
static const RAND_METHOD *old_rand;

int change_rand(void)
{
    /* save old rand method */
    if ((old_rand = RAND_get_rand_method()) == NULL)
        return 0;

    fake_rand.seed = old_rand->seed;
    fake_rand.cleanup = old_rand->cleanup;
    fake_rand.add = old_rand->add;
    fake_rand.status = old_rand->status;
    /* use own random function */
    fake_rand.bytes = fbytes;
    fake_rand.pseudorand = old_rand->bytes;
    /* set new RAND_METHOD */
    if (!RAND_set_rand_method(&fake_rand))
        return 0;
    return 1;
}

int restore_rand(void)
{
    if (!RAND_set_rand_method(old_rand))
        return 0;
    else
        return 1;
}

static int fbytes_counter = 0, use_fake = 0;
static const char *numbers[8] = {
    "651056770906015076056810763456358567190100156695615665659",
    "6140507067065001063065065565667405560006161556565665656654",
    "8763001015071075675010661307616710783570106710677817767166"
    "71676178726717",
    "7000000175690566466555057817571571075705015757757057795755"
    "55657156756655",
    "1275552191113212300012030439187146164646146646466749494799",
    "1542725565216523985789236956265265265235675811949404040041",
    "1456427555219115346513212300075341203043918714616464614664"
    "64667494947990",
    "1712787255652165239672857892369562652652652356758119494040"
    "40041670216363"};

int fbytes(unsigned char *buf, int num)
{
    int ret;
    BIGNUM *tmp = NULL;

    if (use_fake == 0)
        return old_rand->bytes(buf, num);

    use_fake = 0;

    if (fbytes_counter >= 8)
        return 0;
    tmp = BN_new();
    if (!tmp)
        return 0;
    if (!BN_dec2bn(&tmp, numbers[fbytes_counter]))
    {
        BN_free(tmp);
        return 0;
    }
    fbytes_counter++;
    if (num != BN_num_bytes(tmp) || !BN_bn2bin(tmp, buf))
        ret = 0;
    else
        ret = 1;
    BN_free(tmp);
    return ret;
}

/* some tests from the X9.62 draft */
int x9_62_test_internal(BIO *out, int nid, const char *r_in, const char *s_in)
{
    int ret = 0;
    const char message[] = "abc";
    unsigned char digest[20];
    unsigned int dgst_len = 0;
    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();
    EC_KEY *key = NULL;
    ECDSA_SIG *signature = NULL;
    BIGNUM *r = NULL, *s = NULL;
    BIGNUM *kinv = NULL, *rp = NULL;
    const BIGNUM *sig_r, *sig_s;

    if (md_ctx == NULL)
        goto x962_int_err;

    /* get the message digest */
    if (!EVP_DigestInit(md_ctx, EVP_sha1()) || !EVP_DigestUpdate(md_ctx, (const void *)message, 3) || !EVP_DigestFinal(md_ctx, digest, &dgst_len))
        goto x962_int_err;

    BIO_printf(out, "testing %s: ", OBJ_nid2sn(nid));
    /* create the key */
    if ((key = EC_KEY_new_by_curve_name(nid)) == NULL)
        goto x962_int_err;
    use_fake = 1;
    if (!EC_KEY_generate_key(key))
        goto x962_int_err;
    BIO_printf(out, ".");
    (void)BIO_flush(out);
    /* create the signature */
    use_fake = 1;
    // /* Use ECDSA_sign_setup to avoid use of ECDSA nonces */
    if (!ECDSA_sign_setup(key, NULL, &kinv, &rp))
        goto x962_int_err;
    signature = ECDSA_do_sign_ex(digest, 20, kinv, rp, key);
    // signature = ECDSA_do_sign(digest, 20, key);
    if (signature == NULL)
        goto x962_int_err;
    // BIO_printf(out, ".");
    // (void)BIO_flush(out);
    // /* compare the created signature with the expected signature */
    // if ((r = BN_new()) == NULL || (s = BN_new()) == NULL)
    //     goto x962_int_err;
    // if (!BN_dec2bn(&r, r_in) || !BN_dec2bn(&s, s_in))
    //     goto x962_int_err;
    // ECDSA_SIG_get0(signature, &sig_r, &sig_s);
    // if (BN_cmp(sig_r, r) || BN_cmp(sig_s, s))
    //     goto x962_int_err;
    // BIO_printf(out, ".");
    // (void)BIO_flush(out);
    // /* verify the signature */
    // if (ECDSA_do_verify(digest, 20, signature, key) != 1)
    //     goto x962_int_err;
    // BIO_printf(out, ".");
    // (void)BIO_flush(out);

    BIO_printf(out, " ok\n");
    ret = 1;
x962_int_err:
    if (!ret)
        BIO_printf(out, " failed\n");
    EC_KEY_free(key);
    ECDSA_SIG_free(signature);
    BN_free(r);
    BN_free(s);
    EVP_MD_CTX_free(md_ctx);
    BN_clear_free(kinv);
    BN_clear_free(rp);
    return ret;
}

int x9_62_tests(BIO *out)
{
    int ret = 0;

    BIO_printf(out, "some tests from X9.62:\n");

    /* set own rand method */
    if (!change_rand())
        goto x962_err;

    if (!x9_62_test_internal(out, NID_X9_62_prime192v1,
                             "3342403536405981729393488334694600415596881826869351677613",
                             "5735822328888155254683894997897571951568553642892029982342"))
        goto x962_err;
    //     if (!x9_62_test_internal(out, NID_X9_62_prime239v1,
    //                              "3086361431751678114926225473006680188549593787585317781474"
    //                              "62058306432176",
    //                              "3238135532097973577080787768312505059318910517550078427819"
    //                              "78505179448783"))
    //         goto x962_err;
    // # ifndef OPENSSL_NO_EC2M
    //     if (!x9_62_test_internal(out, NID_X9_62_c2tnb191v1,
    //                              "87194383164871543355722284926904419997237591535066528048",
    //                              "308992691965804947361541664549085895292153777025772063598"))
    //         goto x962_err;
    //     if (!x9_62_test_internal(out, NID_X9_62_c2tnb239v1,
    //                              "2159633321041961198501834003903461262881815148684178964245"
    //                              "5876922391552",
    //                              "1970303740007316867383349976549972270528498040721988191026"
    //                              "49413465737174"))
    //         goto x962_err;
    // #endif
    ret = 1;
x962_err:
    if (!restore_rand())
        ret = 0;
    return ret;
}

int main(void)
{
    int ret = 1;
    BIO *out;
    char *p;

    out = BIO_new_fp(stdout, BIO_NOCLOSE | BIO_FP_TEXT);

    p = getenv("OPENSSL_DEBUG_MEMORY");
    if (p != NULL && strcmp(p, "on") == 0)
        CRYPTO_set_mem_debug(1);

    /* initialize the prng */
    RAND_seed(rnd_seed, sizeof(rnd_seed));

    /* the tests */
    if (!x9_62_tests(out))
        goto err;
    // if (!test_builtin(out))
    //     goto err;

    ret = 0;
err:
    if (ret)
        BIO_printf(out, "\nECDSA test failed\n");
    else
        BIO_printf(out, "\nECDSA test passed\n");
    if (ret)
        ERR_print_errors(out);

    // #ifndef OPENSSL_NO_CRYPTO_MDEBUG
    //     if (CRYPTO_mem_leaks(out) <= 0)
    //         ret = 1;
    // #endif
    BIO_free(out);
    return ret;
}
#endif
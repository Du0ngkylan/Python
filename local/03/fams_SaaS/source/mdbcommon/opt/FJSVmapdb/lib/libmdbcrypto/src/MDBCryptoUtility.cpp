/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBCryptoUtility.cpp
 * @brief Implement for crypto utility
 * @author trangvp:
 * @date 2016/11/09
 */

#include <Poco/Crypto/Cipher.h>
#include <Poco/Crypto/CipherKey.h>
#include <Poco/Crypto/CipherFactory.h>

#include "MDBCryptoUtility.h"
#include "MDBConstants.h"
#include "MDBLogger.h"

namespace mdbcom {

    std::string MDBCryptoUtility::mCryptoAlgorithm { "aes256" };
    std::string MDBCryptoUtility::mCryptoSecretKey { "M-IoT_System" };
    std::string MDBCryptoUtility::mCryptoSalt { "@#_M-IoT_!" };

    /**
     * @brief Encrypt an input data using AES 256.
     *
     * @param [in] plainText the text need to encrypt
     * @param [out] encryptedText the encrypted text with base64 encoded
     *
     * @retval Result::OK encrypt successfully
     * @retval Result::NG encryption failed
     */
    int MDBCryptoUtility::encryptAES256(const std::string &plainText, std::string &encryptedText)
    {
        mdb_apl_log_start();

        auto ret = Result::OK; /* pgr0745 */

        try
        {
            const auto &fuzzyKey = makeFuzzySecretKey(mCryptoSecretKey); /* pgr0745 */
            auto cipherKey = Poco::Crypto::CipherKey {mCryptoAlgorithm, fuzzyKey, mCryptoSalt}; /* pgr0745 */
            auto pCipher = Poco::Crypto::Cipher::Ptr   /* pgr0745 */
            {
                Poco::Crypto::CipherFactory::defaultFactory().createCipher(cipherKey)
            };
            encryptedText = pCipher->encryptString(plainText, Poco::Crypto::Cipher::ENC_BASE64_NO_LF);
        }
        catch (...)
        {
            ret = Result::NG;
            auto tmpPlainText = plainText.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Fail to encrypt: %s", tmpPlainText);

        }

        mdb_apl_log_debug("out ret = [%d]", ret);
        mdb_apl_log_end();
        return ret;
    }

    /**
     * @brief Decrypt an input data using AES 256.
     *
     * @param [in] encryptedText the encrypted text with base64 encoded
     * @param [out] plainText the decrypted text
     *
     * @retval Result::OK decrypt successfully
     * @retval Result::NG decryption failed
     */
    int MDBCryptoUtility::decryptAES256(const std::string &encryptedText, std::string &plainText)
    {
        mdb_apl_log_start();

        auto ret = Result::OK; /* pgr0745 */

        try
        {
            const auto &fuzzyKey = makeFuzzySecretKey(mCryptoSecretKey);
            auto cipherKey = Poco::Crypto::CipherKey { mCryptoAlgorithm, fuzzyKey, mCryptoSalt }; /* pgr0745 */
            auto pCipher = Poco::Crypto::Cipher::Ptr
            {
                Poco::Crypto::CipherFactory::defaultFactory().createCipher(cipherKey) };
            plainText = pCipher->decryptString(encryptedText,
                                               Poco::Crypto::Cipher::ENC_BASE64_NO_LF);
        }
        catch (...)
        {
            ret = Result::NG;
            auto tmpEncryptedText = encryptedText.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Fail to decrypt: %s", tmpEncryptedText);

        }

        mdb_apl_log_debug("out ret = [%d]", ret);
        mdb_apl_log_end();
        return ret;
    }
}


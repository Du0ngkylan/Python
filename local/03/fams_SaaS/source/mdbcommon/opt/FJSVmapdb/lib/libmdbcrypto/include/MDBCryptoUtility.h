/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBCryptoUtility.h
 * @brief Definition for crypto utility
 * @author trangvp:
 * @date 2016/11/09
 */

#ifndef LIBMDBCRYPTO_MDBCRYPTOUTILITY_H_
#define LIBMDBCRYPTO_MDBCRYPTOUTILITY_H_

#include <string>

namespace mdbcom {
    /*!
     * @class MDBCryptoUtility
     * @brief Crypto function utility
     */
    class MDBCryptoUtility {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBCryptoUtility(void) = delete;

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBCryptoUtility(void) = delete;

        /*!
         * @brief Copy Constructor.
         * @param None.
         * @return None.
         */
        MDBCryptoUtility(const MDBCryptoUtility &) = delete;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return None.
         */
        MDBCryptoUtility &operator =(const MDBCryptoUtility &) = delete; /* pgr0055 */ /* pgr0309 */

        /**
         * @brief Encrypt an input data using AES 256.
         *
         * @param [in] plainText the text need to encrypt
         * @param [out] encryptedText the encrypted text with base64 encoded
         *
         * @retval Result::OK encrypt successfully
         * @retval Result::NG encryption failed
         */
        static int encryptAES256(const std::string &plainText, std::string &encryptedText);

        /**
         * @brief Decrypt an input data using AES 256.
         *
         * @param [in] encryptedText the encrypted text with base64 encoded
         * @param [out] plainText the decrypted text
         *
         * @retval Result::OK decrypt successfully
         * @retval Result::NG decryption failed
         */
        static int decryptAES256(const std::string &encryptedText, std::string &plainText);
    private:
        //! Crypto algorithm AES 256
        static std::string mCryptoAlgorithm;
        //! Crypto secret key
        static std::string mCryptoSecretKey;
        //! Crypto salt string
        static std::string mCryptoSalt;

        /*!
         * @brief make secret key fuzzy
         * @param [in] secretKey input secret key
         * @return the other version of secret key
         */
        static inline std::string makeFuzzySecretKey(const std::string &secretKey) {
            auto fuzzySecretKey = std::string { secretKey }; /* pgr0745 */
            static char fuzzyPoints[3] { 'Q', 'M', 'G' };

            for (auto i = 0u; i < secretKey.size(); i++) { /* pgr0745 */
                fuzzySecretKey[i] = secretKey[i]
                                    ^ fuzzyPoints[i % (sizeof(fuzzyPoints) / sizeof(char))];
            }

            return fuzzySecretKey;
        }
    };
}

#endif /* LIBMDBCRYPTO_MDBCRYPTOUTILITY_H_ */

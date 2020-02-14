/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBResourceMapper.cpp
 * @brief       Source file for implementing ResourceItem class.
 * @author      trangvp: create it for support ResourceItem code.
 * @date        2016/10/14
 */

#include "MDBMemoryUtility.h"
#include "MDBLogger.h"
#include "MDBException.h"
#include "MDBResourceMapper.h"

#include "MDBDynamicDatabaseAccess.h"

namespace mdbcom {

    /*!
     * @class Impl
     * @brief A pool of resources of threads
     */
    class MDBResourceMapper::Impl {
    public:
        /**
         * @brief Data constructor.
         * Construct resource map for database connections.
         *
         * @param [in] size Size of resource map.
         * @param [in] dynamicPrepareStatements Prepare statements for dynamic Db.
         * @return None.
         */
        Impl(int size, const PrepareStatements *dynamicPrepareStatements);

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~Impl(void) = default;

        /**
         * @brief Assignment constructor.
         *
         * Not support.
         *
         */
        Impl(const Impl &) = delete; /*pgr0571*/ /*pgr2222*/

        /**
         * @brief Assignment operation.
         *
         * Not support.
         *
         */
        Impl &operator=(const Impl &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /**
         * @brief Get connection
         *
         * Get connection for dynamic database
         *
         * @retval nullptr  If source have no connection.
         * @retval Pointer to database connection.
         */
        inline std::unique_ptr<const MDBDatabaseAccess> &getDynamicDBConn(void) {
            return getConnection(dynamicConnections);
        }

        /**
         * @brief Check resource
         *
         * Check if current resource contain Dynamic DB connection
         *
         * @param None.
         * @retval true  If using Dynamic database
         * @retval false If not using Dynamic database
         */
        inline bool isUsingDynamicDB(void) const {
            return (nullptr != dynamicStatements);
        }

        //! List of connections type
        using ConnectionVector = std::vector<std::unique_ptr<const MDBDatabaseAccess>>;

        /**
         * @brief Get database connection.
         *
         * Get database connection from specify source.
         *
         * @param [in,out] source Source to get connection from.
         * @retval nullptr  If source have no connection.
         * @retval Pointer to database connection.
         */
        std::unique_ptr<const MDBDatabaseAccess> &getConnection(ConnectionVector &source);

        inline const PrepareStatements *getDynamicStatements() const {
            return dynamicStatements;
        }

    private:

        //! Connections list of Dynamic DB
        ConnectionVector dynamicConnections;

        //! Prepare statements list of Dynamic DB
        const PrepareStatements *dynamicStatements;
    };

    //---------------------------- Impl-----------------------------

    MDBResourceMapper::Impl::Impl(int size, const PrepareStatements *dynamicPrepareStatements) :
        dynamicConnections { }, dynamicStatements { dynamicPrepareStatements }
    {

        mdb_apl_log_start();

        // Preparation space for connections source
        if (nullptr != dynamicStatements)
        {
            dynamicConnections.reserve(size);
        }

        for (auto i = 0; i < size; ++i)  /* pgr0692 *//* pgr0745 */
        {
            // Initialize each connections in source
            if (nullptr != dynamicStatements)
            {
                auto connection = std::unique_ptr<MDBDatabaseAccess> { nullptr }; /* pgr0745 */

                try
                {
                    // Initialize Dynamic database connection
                    connection = MDBMemoryUtility::make_unique<MDBDynamicDatabaseAccess>();
                }
                catch (const std::bad_alloc &ex)
                {
                    auto exceptionMessage = ex.what(); /* pgr0745 */
                    mdb_apl_log_critical("Can't create connection for dyanmic db [index:%d][%s]", i,
                                         exceptionMessage);
                    mdb_apl_log_end();
                    throw;
                }
                catch (const MDBException &ex)
                {
                    auto exceptionMessage = ex.what(); /* pgr0745 */
                    mdb_apl_log_critical("Can't create connection for dynamic db [index:%d][%s]", i,
                                         exceptionMessage);
                    mdb_apl_log_end();
                    throw;
                }
                catch (...)
                {
                    mdb_apl_log_critical("Can't create connection for dynamic db [index:%d]", i);
                    mdb_apl_log_end();
                    throw MDBException("Error creating connection to dynamic db");
                }

                if (Result::OK != connection->prepareAll(*dynamicStatements))
                {
                    mdb_apl_log_critical("Can't prepare connection for dynamic db [index:%d]", i);
                    mdb_apl_log_end();
                    throw MDBException("Can't prepare connection for dynamic db");
                }
                else
                {
                    // convert to const MDBDatabaseAccess
                    auto preparedConnection = std::unique_ptr<const MDBDatabaseAccess> /*pgr0745*/
                    {
                        connection.release()
                    }; /* pgr0745 */
                    // push to connections source for later usage
                    dynamicConnections.push_back(std::move(preparedConnection));
                }
            }
        }

        mdb_apl_log_end();
    } /* pgr0840 */

    std::unique_ptr<const MDBDatabaseAccess> &MDBResourceMapper::Impl::getConnection(
        ConnectionVector &source)   /* pgr0304 */
    {
        mdb_apl_log_start();

        if (true == source.empty())
        {
            mdb_apl_log_critical("Request on empty resource");
            mdb_apl_log_end();
            throw MDBException("Source not holding any resource.");
        }

        while (nullptr == source.back())
        {
            // connection has been retrieved by user, remove from source
            source.pop_back();

            // if there is no more resource, requesting more than capability
            if (true == source.empty())
            {
                mdb_apl_log_critical("Request on empty resource");
                mdb_apl_log_end();
                throw MDBException("Source not holding any resource.");
            }
        }

        mdb_apl_log_end();
        return source.back();
    }

    //---------------------------- MDBResourceMapper-----------------------------

    /**
     * @brief Data constructor.
     * Construct resource map for database connections.
     *
     * @param [in] size Size of resource map (number of connections for each database)
     * @param [in] dynamicPrepareStatements Prepare statements for dynamic Db.
     * @return None.
     */
    MDBResourceMapper::MDBResourceMapper(int size, const PrepareStatements *dynamicPrepareStatements) :
        pImpl { MDBMemoryUtility::make_unique<Impl>(size, dynamicPrepareStatements) }
    {

    }
    /*!
     * @brief Destructor.
     * @param None.
     * @return None.
     */
    MDBResourceMapper::~MDBResourceMapper(void) = default;

    /**
     * @brief Get connection
     *
     * Get connection for dynamic database
     *
     * @retval nullptr  If source have no connection.
     * @retval Pointer to database connection.
     */
    std::unique_ptr<const MDBDatabaseAccess> &MDBResourceMapper::getDynamicDBConn(void)
    {
        return pImpl->getDynamicDBConn();
    }

    /**
     * @brief Check resource
     *
     * Check if current resource contain dynamic DB connection
     *
     * @param None.
     * @retval true  If using dynamic database
     * @retval false If not using dynamic database
     */
    bool MDBResourceMapper::isUsingDynamicDB(void) const
    {
        return (nullptr != pImpl->getDynamicStatements());
    }

}


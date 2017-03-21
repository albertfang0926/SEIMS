/*!
 * \brief Utility functions of MongoDB
 * Currently, mongo-c-driver 1.5.0 is supported.
 * \author Junzhi Liu, LiangJun Zhu
 * \date May 2016
 * \revised Feb 2017
 * 
 */
#ifndef MONGO_UTILS
#define MONGO_UTILS

#include <mongoc.h>
#include <vector>
#include <string>
#include <set>
#include "utilities.h"

using namespace std;

//template<typename T> class clsRasterData;

/*!
 * \brief Macro definitions of Raster of Watershed modeling datasets for I/O in MongoDB
 * \version 1.0
 * \author Liangjun Zhu
 */
#define MONG_GRIDFS_FN                "filename"
#define MONG_GRIDFS_WEIGHT_CELLS    "NUM_CELLS"
#define MONG_GRIDFS_WEIGHT_SITES    "NUM_SITES"
#define MONG_GRIDFS_ID                "ID"
#define MONG_GRIDFS_SUBBSN            "SUBBASIN"

/*!
 * \class MongoClient
 * \brief Create a MongoDB Client
 */
class MongoClient {
public:
    /*!
     * \brief Constructor
     * Get a client for a MongoDB instance using ip address and port number
     */
    MongoClient(const char *host, int port);

    /*!
     * \brief Destructor
     */
    ~MongoClient();

    /*!
     * \brief Get mongoc_client instance
     */
    mongoc_client_t *getConn() { return m_conn; }

    /*!
     * \brief Get existed Database instance
     * Besides, Databases are automatically created on the MongoDB server
     * upon insertion of the first document into a collection.
     * Therefore, there is no need to create a database manually.
     */
    mongoc_database_t *getDatabase(string const &dbname);

    /*!
     * \brief Get Collection instance
     */
    mongoc_collection_t *getCollection(string const &dbname, string const &collectionname);

    /*!
     * \brief Get GridFS instance
     */
    mongoc_gridfs_t *getGridFS(string const &dbname, string const &gfsname);

    /*!
     * \brief Get database names
     * \return Database names, vector<string>
     */
    vector <string> getDatabaseNames();

    /*!
     * \brief Get collection names in MongoDB database
     * \param[in] dbName \string database name
     */
    vector <string> getCollectionNames(string const &dbName);

    /*!
     * \brief Get GridFs file names in MongoDB database
     * \param[in] dbName \string database name
     * \param[in] gfs \string GridFS name
     * \return filenames vector<string>
     */
    vector <string> getGridFSFileNames(string const &dbname, string const &gfsname);

private:
    /*!
     * \brief Get database names
     */
    void _database_names();

private:
    const char *m_host;
    int m_port;
    mongoc_client_t *m_conn;
    vector <string> m_dbnames;
};

/*!
 * \class MongoDatabase
 * \brief Create a MongoDB database instance
 */
class MongoDatabase {
public:
    /*!
     * \brief Constructor, initialized by a mongoc_database_t* pointer
     */
    MongoDatabase(mongoc_database_t *db);

    /*!
     * \brief Constructor, initialized by mongodb client and database name
     */
    MongoDatabase(mongoc_client_t *conn, string const &dbname);

    /*!
     * \brief Destructor by Destroy function
     */
    ~MongoDatabase();

    /*!
      * \brief Get collection names in current database
      */
    vector <string> getCollectionNames();

private:
    mongoc_database_t *m_db;
    string m_dbname;
};

class MongoCollection {
    // TODO
};

/*!
 * \class MongoGridFS
 * \brief Create a MongoDB GridFS instance
 */
class MongoGridFS {
public:
    /*!
     * \brief Constructor, initialized by a mongoc_gridfs_t* pointer or NULL
     */
    MongoGridFS(mongoc_gridfs_t *gfs = NULL);

    /*!
     * \brief Destructor by Destroy function
     */
    ~MongoGridFS();

    mongoc_gridfs_t *getGridFS() { return m_gfs; }

    /*!
     * \brief Get GridFS file by name
     */
    mongoc_gridfs_file_t *getFile(string const &gfilename, mongoc_gridfs_t *gfs = NULL);

    /*!
     * \brief Remove GridFS file by name
     */
    bool removeFile(string const &gfilename, mongoc_gridfs_t *gfs = NULL);

    /*!
     * \brief Get GridFS file names
     */
    vector <string> getFileNames(mongoc_gridfs_t *gfs = NULL);

    /*!
     * \brief Get metadata of a given GridFS file name
     */
    bson_t *getFileMetadata(string const &gfilename, mongoc_gridfs_t *gfs = NULL);

    /*!
     * \brief Get stream data of a given GridFS file name
     */
    void getStreamData(string const &gfilename, char *&databuf, int &datalength, mongoc_gridfs_t *gfs = NULL);

    /*!
     * \brief Write stream data to a GridFS file
     */
    void
    writeStreamData(string const &gfilename, char *&buf, int &length, const bson_t *p, mongoc_gridfs_t *gfs = NULL);

private:
    mongoc_gridfs_t *m_gfs;
};

/*!
 * \brief Get numeric value from \a bson_iter_t according to a given key
 * \param[in] iter \a bson_iter_t
 * \param[in] key 
 * \param[in] numericvalue, int, float, or double
 */
template<typename T>
void GetNumericFromBsonIterator(bson_iter_t *iter, T &numericvalue) {
    try {
        const bson_value_t *vv = bson_iter_value(iter);
        if (vv->value_type == BSON_TYPE_INT32) {
            numericvalue = (T) vv->value.v_int32;
        } else if (vv->value_type == BSON_TYPE_INT64) {
            numericvalue = (T) vv->value.v_int64;
        } else if (vv->value_type == BSON_TYPE_DOUBLE) {
            numericvalue = (T) vv->value.v_double;
        } else if (vv->value_type == BSON_TYPE_UTF8) {
            string tmp = vv->value.v_utf8.str;
            numericvalue = (T) atof(tmp.c_str());
        } else {
            throw ModelException("MongoUtil", "GetNumericFromBsonIterator",
                                 "bson iterator isn't or not contains a numeric value.\n");
        }
    }
    catch (ModelException e) {
        cout << e.toString() << endl;
        exit(EXIT_FAILURE);
    }
}

/*!
 * \brief Get numeric value from \a bson_t according to a given key
 * \param[in] bmeta \a bson_t
 * \param[in] key 
 * \param[in] numericvalue, int, float, or double
 */
template<typename T>
void GetNumericFromBson(bson_t *bmeta, const char *key, T &numericvalue) {
    bson_iter_t iter;
    if (bson_iter_init(&iter, bmeta) && bson_iter_find(&iter, key)) {
        GetNumericFromBsonIterator(&iter, numericvalue);
    } else {
        StatusMessage(("WARNING: GetNumericFromBson, Failed in get value of: " + string(key) + "\n").c_str());
    }
}

/*!
 * \brief Get String from \a bson_iter_t
 * \param[in] bmeta \a bson_iter_t
 * \return Float value if success, or "" if failed.
 */
string GetStringFromBsonIterator(bson_iter_t *iter);

/*!
 * \brief Get String from \a bson_t
 * \param[in] bmeta \a bson_t
 * \param[in] key
 * \return Float value if success, or "" if failed.
 */
string GetStringFromBson(bson_t *bmeta, const char *key);

/*!
 * \brief Get Bool from \a bson_iter_t
 * \param[in] bmeta \a bson_iter_t
 * \return true if success, or false if failed.
 */
bool GetBoolFromBsonIterator(bson_iter_t *iter);

/*!
 * \brief Get String from \a bson_t
 * \param[in] bmeta \a bson_t
 * \param[in] key
 * \return true if success, or false if failed.
 */
bool GetBoolFromBson(bson_t *bmeta, const char *key);

/*!
 * \brief Get Datetime from \a bson_iter_t
 * \param[in] bmeta \a bson_iter_t
 * \return time_t float value if success, or -1 if failed.
 */
time_t GetDatetimeFromBsonIterator(bson_iter_t *iter);

/*!
 * \brief Get Datetime from \a bson_t
 * \param[in] bmeta \a bson_t
 * \param[in] key
 * \return time_t float value if success, or -1 if failed.
 */
time_t GetDatetimeFromBson(bson_t *bmeta, const char *key);

#endif
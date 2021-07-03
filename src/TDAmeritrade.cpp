#include "TDAmeritrade.hpp"

namespace tda
{
    TDAmeritrade::TDAmeritrade( RetrievalType type )
    {
        switch ( type )
        {
            case QUOTE:
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + TDA_API_KEY;
                break;
            case PRICE_HISTORY:
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY + "&periodType={periodType}&period={period}&frequencyType={frequencyType}&frequency={frequency}";
                break;
            default:
                break;
        }

        this->_period_type = DAY;
        this->_frequency_type = MINUTE;
        this->_col_name = "Open";
    }

    std::string TDAmeritrade::get_api_interval_value(int value)
    {
        return EnumAPIValues[value];
    }

    std::string TDAmeritrade::timestamp_from_string(std::string date)
    {
        struct std::tm time = {0,0,0,0,0,0,0,0,0};
        std::istringstream ss(date);
        ss >> std::get_time(&time, "%Y-%m-%d");
        if ( ss.fail() )
        {
            std::cerr  << "ERROR: Cannot parse date string (" << date <<"); required format %Y-%m-%d" << std::endl;
            exit(1);
        } 
        time.tm_hour = 0;
        time.tm_min = 0;
        time.tm_sec = 0;
        std::time_t epoch = std::mktime(&time);

        return std::to_string(epoch);
    }

    bool TDAmeritrade::string_replace(std::string& str, const std::string from, const std::string to)
    {
        size_t start = str.find(from);
        if ( start == std::string::npos )
        {
            return false;
        }

        str.replace(start, from.length(), to);
        return true;
    }

    std::string TDAmeritrade::build_url(std::string ticker, std::string start, std::string end)
    {
        std::string url = this->_base_url;
        string_replace(url, "{ticker}", ticker);
        string_replace(url, "{start_time}", timestamp_from_string(start));
        string_replace(url, "{end_time}", timestamp_from_string(end));
        string_replace(url, "{periodType}", get_api_interval_value(this->_period_type));
        return url;
    }

    void TDAmeritrade::download_file(std::string url, std::string filename)
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            fp = fopen(filename.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);

            /* always cleanup */
            curl_easy_cleanup(curl);
            fclose(fp);
        }
    }

    void TDAmeritrade::set_period_type(PeriodType periodType)
    {
        this->_period_type = periodType;
    }

    void TDAmeritrade::set_col_name( std::string name )
    {
        this->_col_name = name;
    }

    boost::shared_ptr<tda::Quote> TDAmeritrade::createQuote( std::string ticker )
    {

        std::string url = this->_base_url;
        string_replace(url, "{ticker}", ticker);

        std::time_t now = std::time(0);
        std::string output_file_name = ticker + "_" + std::to_string(now) + ".json";

        download_file(url, output_file_name);

        std::ifstream jsonFile(output_file_name);

        boost::property_tree::ptree propertyTree;
        read_json(jsonFile, propertyTree);

        boost::shared_ptr<Quote> new_quote_data = boost::make_shared<Quote>( propertyTree );

        std::remove(output_file_name.c_str());

        return new_quote_data;
    }

    void TDAmeritrade::retrieveQuoteData( std::string ticker, bool keep_file )
    {
        std::string url = this->_base_url;
        string_replace(url, "{ticker}", ticker);

        std::time_t now = std::time(0);
        std::string output_file_name = ticker + "_" + std::to_string(now) + ".json";

        download_file(url, output_file_name);

        std::ifstream jsonFile(output_file_name);

        boost::property_tree::ptree propertyTree;
        read_json(jsonFile, propertyTree);

        for (auto & array_element: propertyTree) 
        {
            for (auto & property: array_element.second) 
            {
                std::cout << property.first << " = " << property.second.get_value < std::string > () << "\n";
            }
        }

        if ( !keep_file )
        {
            std::remove(output_file_name.c_str());
        }
    }

    /* =============== Quote Class =============== */

    void Quote::initVariables()
    {
        for (auto & array_element: this->quoteData ) 
        {
            for (auto & property: array_element.second) 
            {
                quoteVariables[ property.first ] = property.second.get_value < std::string > ();
            }
        }
    }

    Quote::Quote( boost::property_tree::ptree quote_data )
    {
        this->quoteData = quote_data;

        initVariables();
    }

    std::string Quote::getQuoteVariable( std::string variable )
    {
        return this->quoteVariables[variable];
    }
}
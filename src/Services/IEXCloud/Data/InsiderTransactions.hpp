#ifndef InsiderTransactions_hpp
#define InsiderTransactions_hpp

#include "../../../core.hpp"

namespace iex
{
    // conversionOrExercisePrice 	number 	The conversion or exercise price of the transaction, if available
    // directIndirect 	            letter 	(D)irect or (I)ndirect
    // effectiveDate 	            number 	Effective date of the transaction.
    // filingDate 	                string 	Date the transaction was filed with the SEC.
    // fullName 	                string 	Full name of the individual. This field concatenates the individuals First Name, Middle Name, Last Name and Suffix.
    // is10b51 	                    boolean 	Whether the transaction was executed under Rule 10b5-1. Rule 10b5-1 allows company insiders to make predetermined trades while following insider trading laws and avoiding insider trading accusations. Learn more.
    // postShares 	                number 	The reported number of shares held after the transaction.
    // reportedTitle 	            string 	Insiders job title per the sourced filing if available
    // symbol 	                    string 	Associated ticker or symbol
    // transactionCode 	            letter 	Transaction Codes
    // transactionDate 	            string 	Date the transaction was executed
    // transactionPrice 	        number 	As-reported (unadjusted) unit price at which shares were acquired or disposed, represented in USD.
    // transactionShares 	        number 	As-reported (unadjusted) number of shares acquired or disposedValue of the transaction, calculated as Tran_Shares * Tran_Price, represented in USD. This value is not adjusted for corporate actions.
    // transactionValue 	        number 	Value of the transaction, calculated as Tran_Shares * Tran_Price, represented in USD. This value is not adjusted for corporate actions.
    // tranPrice 	                number 	As-reported (unadjusted) unit price at which shares were acquired or disposed, represented in USD.
    // tranShares 	                number 	As-reported (unadjusted) number of shares acquired or disposedValue of the transaction, calculated as Tran_Shares * Tran_Price, represented in USD. This value is not adjusted for corporate actions.
    // tranValue 	                number 	Value of the transaction, calculated as Tran_Shares * Tran_Price, represented in USD. This value is not adjusted for corporate actions.
    class InsiderTransactions
    {
    private:
        double conversionOrExercisePrice; 
        char directIndirect;
        double effectiveDate;
        std::string filingDate;
        std::string fullName;
        bool is10b51;
        double postShares;
        std::string reportedTitle;
        std::string symbol;
        char transactionCode;
        std::string transactionDate;
        double transactionPrice;
        double transactionShares;
        double transactionValue;
        double tranPrice;
        double tranShares;
        double tranValue;

    public:
        InsiderTransactions() { }

        double getTranValue() const { return tranValue; }
        void setTranValue(double tranValue_) { tranValue = tranValue_; }

        double getTranShares() const { return tranShares; }
        void setTranShares(double tranShares_) { tranShares = tranShares_; }

        double getTranPrice() const { return tranPrice; }
        void setTranPrice(double tranPrice_) { tranPrice = tranPrice_; }

        double getTransactionValue() const { return transactionValue; }
        void setTransactionValue(double transactionValue_) { transactionValue = transactionValue_; }

        double getTransactionShares() const { return transactionShares; }
        void setTransactionShares(double transactionShares_) { transactionShares = transactionShares_; }

        char getTransactionCode() const { return transactionCode; }
        void setTransactionCode(char transactionCode_) { transactionCode = transactionCode_; }

        double getPostShares() const { return postShares; }
        void setPostShares(double postShares_) { postShares = postShares_; }

        std::string getFullName() const { return fullName; }
        void setFullName(const std::string &fullName_) { fullName = fullName_; }

        double getConversionOrExercisePrice() const { return conversionOrExercisePrice; }
        void setConversionOrExercisePrice(double conversionOrExercisePrice_) { conversionOrExercisePrice = conversionOrExercisePrice_; }
    };
}

#endif
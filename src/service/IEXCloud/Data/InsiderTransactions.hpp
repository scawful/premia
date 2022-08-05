#ifndef InsiderTransactions_hpp
#define InsiderTransactions_hpp

#include "premia.h"
namespace premia {
namespace iex {
// conversionOrExercisePrice 	number 	The conversion or exercise price of the
// transaction, if available
// directIndirect 	            letter 	(D)irect or (I)ndirect
// effectiveDate 	            number 	Effective date of the
// transaction. filingDate 	                CRString 	Date the
// transaction was filed with the SEC.
// fullName 	                CRString 	Full name of the individual. This
// field concatenates the individuals First Name, Middle Name, Last Name and
// Suffix. is10b51 	                    boolean 	Whether the transaction
// was executed under Rule 10b5-1. Rule 10b5-1 allows company insiders to make
// predetermined trades while following insider trading laws and avoiding
// insider trading accusations. Learn more. postShares number 	The reported
// number of shares held after the transaction. reportedTitle CRString Insiders
// job title per the sourced filing if available symbol CRString
// Associated ticker or symbol
// transactionCode 	            letter 	Transaction Codes
// transactionDate 	            CRString 	Date the transaction was
// executed transactionPrice 	        number 	As-reported (unadjusted) unit
// price at which shares were acquired or disposed, represented in USD.
// transactionShares 	        number 	As-reported (unadjusted) number of
// shares acquired or disposedValue of the transaction, calculated as
// Tran_Shares * Tran_Price, represented in USD. This value is not adjusted for
// corporate actions. transactionValue 	        number 	Value of the
// transaction, calculated as Tran_Shares * Tran_Price, represented in USD. This
// value is not adjusted for corporate actions. tranPrice number
// As-reported (unadjusted) unit price at which shares were acquired or
// disposed, represented in USD. tranShares 	                number
// As-reported (unadjusted) number of shares acquired or disposedValue of the
// transaction, calculated as Tran_Shares * Tran_Price, represented in USD. This
// value is not adjusted for corporate actions. tranValue number 	Value of
// the transaction, calculated as Tran_Shares * Tran_Price, represented in USD.
// This value is not adjusted for corporate actions.
class InsiderTransactions {
 private:
  double conversionOrExercisePrice;
  char directIndirect;
  double effectiveDate;
  String filingDate;
  String fullName;
  bool is10b51;
  double postShares;
  String reportedTitle;
  String symbol;
  char transactionCode;
  String transactionDate;
  double transactionPrice;
  double transactionShares;
  double transactionValue;
  double tranPrice;
  double tranShares;
  double tranValue;

 public:
  InsiderTransactions() = default;

  double getTranValue() const { return tranValue; }
  void setTranValue(double tranValue_) { tranValue = tranValue_; }

  double getTranShares() const { return tranShares; }
  void setTranShares(double tranShares_) { tranShares = tranShares_; }

  double getTranPrice() const { return tranPrice; }
  void setTranPrice(double tranPrice_) { tranPrice = tranPrice_; }

  double getTransactionValue() const { return transactionValue; }
  void setTransactionValue(double transactionValue_) {
    transactionValue = transactionValue_;
  }

  double getTransactionShares() const { return transactionShares; }
  void setTransactionShares(double transactionShares_) {
    transactionShares = transactionShares_;
  }

  char getTransactionCode() const { return transactionCode; }
  void setTransactionCode(char transactionCode_) {
    transactionCode = transactionCode_;
  }

  double getPostShares() const { return postShares; }
  void setPostShares(double postShares_) { postShares = postShares_; }

  String getFullName() const { return fullName; }
  void setFullName(CRString fullName_) { fullName = fullName_; }

  double getConversionOrExercisePrice() const {
    return conversionOrExercisePrice;
  }
  void setConversionOrExercisePrice(double conversionOrExercisePrice_) {
    conversionOrExercisePrice = conversionOrExercisePrice_;
  }

  double getTransactionPrice() const { return transactionPrice; }
  void setTransactionPrice(double transactionPrice_) {
    transactionPrice = transactionPrice_;
  }

  bool getIs10b51() const { return is10b51; }
  void setIs10b51(bool is10b51_) { is10b51 = is10b51_; }

  double getEffectiveDate() const { return effectiveDate; }
  void setEffectiveDate(double effectiveDate_) {
    effectiveDate = effectiveDate_;
  }

  char getDirectIndirect() const { return directIndirect; }
  void setDirectIndirect(char directIndirect_) {
    directIndirect = directIndirect_;
  }
};
}  // namespace iex
}
#endif
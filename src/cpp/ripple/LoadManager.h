#ifndef LOADSOURCE__H
#define LOADSOURCE__H

#include <boost/thread/mutex.hpp>

#include "types.h"

class LoadSource
{ // a single endpoint that can impose load
	friend class LoadManager;

public:

	// load source flags
	static const int lsfPrivileged	= 1;

protected:
	int		mBalance;
	int		mFlags;
	time_t	mLastUpdate;
	time_t	mLastWarning;

public:
	LoadSource() : mBalance(0), mFlags(0), mLastWarning(0)
									{ mLastUpdate = time(NULL); }

	bool	isPrivileged() const	{ return (mFlags & lsfPrivileged) != 0; }
	void	setPrivileged()			{ mFlags |= lsfPrivileged; }
	int		getBalance() const		{ return mBalance; }
};


class LoadManager
{ // a collection of load sources
protected:

	int mCreditRate;			// credits gained/lost per second
	int mCreditLimit;			// the most credits a source can have
	int	mDebitWarn;				// when a source drops below this, we warn
	int mDebitLimit;			// when a source drops below this, we cut it off (should be negative)

	mutable boost::mutex mLock;

	void canonicalize(LoadSource&, const time_t now) const;

public:

	LoadManager(int creditRate, int creditLimit, int debitWarn, int debitLimit) :
		mCreditRate(creditRate), mCreditLimit(creditLimit), mDebitWarn(debitWarn), mDebitLimit(debitLimit) { ; }

	int getCreditRate() const;
	int getCreditLimit() const;
	int getDebitWarn() const;
	int getDebitLimit() const;
	void setCreditRate(int);
	void setCreditLimit(int);
	void setDebitWarn(int);
	void setDebitLimit(int);

	bool shouldWarn(LoadSource&) const;
	bool shouldCutoff(LoadSource&) const;
	bool adjust(LoadSource&, int credits) const;	// return value: false = balance okay, true = warn/cutoff
};

class LoadTrack
{ // structure that tracks our current fee/load schedule
protected:

	uint32 mLocalTxnLoadFee;		// Scale factor, 256 = normal fee
	uint32 mRemoteTxnLoadFee;		// Scale factor, 256 = normal fee
	uint32 mPeerLoadSchedule;		// Schedule setting, 0 = normal schedule
	uint32 mClientLoadSchedule;		// Schedule setting, 0 = normal schedule

public:

	LoadTrack() : mLocalTxnLoadFee(256), mRemoteTxnLoadFee(256), mPeerLoadSchedule(0), mClientLoadSchedule(0)
	{ ; }

	uint64 scaleFee(uint64 fee);

	void raiseRemoteFee();
	void raiseLocalFee();
	void lowerRemoteFee();
	void lowerLocalFee();
};


#endif

// vim:ts=4

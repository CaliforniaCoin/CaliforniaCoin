#include "uritests.h"
#include "../guiutil.h"
#include "../walletmodel.h"

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?label=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?amount=100&label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI("californiacoin://CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW"));
    QVERIFY(rv.label == QString());

    // We currently don't implement the message parameter (ok, yea, we break spec...)
    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?req-message=Wikipedia Example Address"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("californiacoin:CQDfEMUzZhT7qLvLhaXREQbuWLRoSRzPiW?amount=1,000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));
}

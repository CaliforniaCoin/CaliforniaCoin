// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        // ( 0, uint256("0x7b548031469229deb9f421b05a01e4a9dbcf9b30aecc6f17058929f7f53f6e26"))
        ( 0, uint256("0x1017569372a0f2c626d602566608fd7e29693c028cb38b3d3ea9bf1a0b93ee1e"))
        (1,uint256("0xe31fafaa991b243f2d1144332ee6198f017a1c72c7688aba7239115a35949816"))
        (2,uint256("0x23ec6be809e7546cb06ce5e328c2ea23163dfec175623109feccc6feab3c63dc"))
        (3,uint256("0x334e3cd1c3e99756bf3b7d9fb611a4f2e26b8c733bb78cf1e225157caa917bc5"))
        (4,uint256("0xb207a2955cd3c83da66cd77760cc5ffc6e026cc0944fe917255b5066e24b2683"))
        (5,uint256("0xa80ad2238de3123fc8237ba3c261b4ed9306d64a051bc1cc90592e31a21df8ae"))
        (6,uint256("0x05ad5fbc50b5ee67dba88027109a1d4237fe8b89bcf10ee82e042459215c20bd"))
        (7,uint256("0x93b5921bdc6899d0c7229bddbee19ca668fc5dffceae9819492580d8654aa13a"))
        (8,uint256("0xce79d261dc9c4f75adc054ef79cbee57a7e2b2b517ce5ec86f2fac7db5fa09dc"))
        (9,uint256("0x09d8f0f8a1e8d98b362efb0dd6268c5a6e515ed14f4b759335846f0e5fd2ad13"))
        (10,uint256("0xa0224a8185f59d6ab6f14c265001170912b3b26fae7c5b9f185eb3d1b06daf7b"))
        (1005,uint256("0x4fc946cd633740ff4bc7b624f65a9ab1d54a50ed1d85b6efff03ca1fe55da345"))
        (1049,uint256("0x43aa25dd33660710dcd39db4b023d1a378969d59435d2cd08bf88b6ef0d260c8"))
        (1336,uint256("0xd8422e5eaedb9996a4736286d64dae2ae2a3652f0d20749c23502e55eb41ad5a"))
        (1389,uint256("0x468ea4dca9114b97ed4c02258f876595ef8c6548070259d68e323603ffdea687"))
        (1627,uint256("0x27ce433f6192471794dbe54d298ab59aaf55cdde4951f2fcdc7c65930c330fb9"))
        (1973,uint256("0x41158f014b17cfd7128eb8b3f5f5e171d38ef9e9a4aaa286733070f22e1f6559"))
        (2025,uint256("0x9f77de3fddd54ede574c08a48f0ab9926a987eebdd975514a086ad0cb48b63a8"))
        (27496,uint256("0xa71f067f976511ecc2ba4696c32c87c2392582717676429de5ccd261508e404c"))
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1402128269, // * UNIX timestamp of last checkpoint block
        32257,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        8000.0     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet = 
        boost::assign::map_list_of
        (   546, uint256("000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70"))
        ( 35000, uint256("2af959ab4f12111ce947479bfcef16702485f04afd95210aa90fde7d1e4a64ad"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1369685559,
        37581,
        300
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (fTestNet) return true; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (fTestNet) return 0; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (fTestNet) return NULL; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}

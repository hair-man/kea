// Copyright (C) 2011  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

// $Id$
#include <config.h>
#include <string>
#include <gtest/gtest.h>
#include <dns/rrset.h>
#include <dns/rcode.h>
#include "resolver_cache.h"
#include "cache_test_messagefromfile.h"

using namespace isc::cache;
using namespace isc::dns;
using namespace std;

namespace {

class NegativeCacheTest: public testing::Test{
public:
    NegativeCacheTest() {
        vector<CacheSizeInfo> vec;
        CacheSizeInfo class_in(RRClass::IN(), 100, 200);
        vec.push_back(class_in);
        cache = new ResolverCache(vec);
    }

    ~NegativeCacheTest() {
        delete cache;
    }

    ResolverCache *cache;
};

TEST_F(NegativeCacheTest, testNXDOMAIN){
    // NXDOMAIN response for nonexist.example.com
    Message msg_nxdomain(Message::PARSE);
    messageFromFile(msg_nxdomain, "message_nxdomain_with_soa.wire");
    cache->update(msg_nxdomain);

    msg_nxdomain.makeResponse();

    Name non_exist_qname("nonexist.example.com.");
    EXPECT_TRUE(cache->lookup(non_exist_qname, RRType::A(), RRClass::IN(), msg_nxdomain));

    RRsetIterator iter = msg_nxdomain.beginSection(Message::SECTION_AUTHORITY);
    RRsetPtr rrset_ptr = *iter;

    // The TTL should equal to the TTL of SOA record
    const RRTTL& nxdomain_ttl1 = rrset_ptr->getTTL();
    EXPECT_EQ(nxdomain_ttl1.getValue(), 86400);

    // SOA response for example.com
    Message msg_example_com_soa(Message::PARSE);
    messageFromFile(msg_example_com_soa, "message_example_com_soa.wire");
    cache->update(msg_example_com_soa);

    msg_example_com_soa.makeResponse();
    Name soa_qname("example.com.");
    EXPECT_TRUE(cache->lookup(soa_qname, RRType::SOA(), RRClass::IN(), msg_example_com_soa));

    iter = msg_example_com_soa.beginSection(Message::SECTION_ANSWER);
    rrset_ptr = *iter;

    // The TTL should equal to the TTL of SOA record in answer section
    const RRTTL& soa_ttl = rrset_ptr->getTTL();
    EXPECT_EQ(soa_ttl.getValue(), 172800);

    // Query nonexist.example.com again
    Message msg_nxdomain2(Message::PARSE);
    messageFromFile(msg_nxdomain2, "message_nxdomain_with_soa.wire");
    msg_nxdomain2.makeResponse();

    EXPECT_TRUE(cache->lookup(non_exist_qname, RRType::A(), RRClass::IN(), msg_nxdomain2));
    iter = msg_nxdomain2.beginSection(Message::SECTION_AUTHORITY);
    rrset_ptr = *iter;

    // The TTL should equal to the TTL of negative response SOA record
    const RRTTL& nxdomain_ttl2 = rrset_ptr->getTTL();
    EXPECT_EQ(nxdomain_ttl2.getValue(), 86400);

    // Check the normal SOA cache again
    Message msg_example_com_soa2(Message::PARSE);
    messageFromFile(msg_example_com_soa2, "message_example_com_soa.wire");
    msg_example_com_soa2.makeResponse();
    EXPECT_TRUE(cache->lookup(soa_qname, RRType::SOA(), RRClass::IN(), msg_example_com_soa2));

    iter = msg_example_com_soa2.beginSection(Message::SECTION_ANSWER);
    rrset_ptr = *iter;
    const RRTTL& soa_ttl2 = rrset_ptr->getTTL();
    // The TTL should equal to the TTL of SOA record in answer section
    EXPECT_EQ(soa_ttl2.getValue(), 172800);
}

TEST_F(NegativeCacheTest, testNXDOMAINWithoutSOA){
    // NXDOMAIN response for nonexist.example.com
    Message msg_nxdomain(Message::PARSE);
    messageFromFile(msg_nxdomain, "message_nxdomain_no_soa.wire");
    cache->update(msg_nxdomain);

    msg_nxdomain.makeResponse();

    Name non_exist_qname("nonexist.example.com.");
    // The message should not be cached
    EXPECT_FALSE(cache->lookup(non_exist_qname, RRType::A(), RRClass::IN(), msg_nxdomain));
}

TEST_F(NegativeCacheTest, testNXDOMAINCname){
    // a.example.org points to b.example.org
    // b.example.org points to c.example.org
    // c.example.org does not exist
    Message msg_nxdomain_cname(Message::PARSE);
    messageFromFile(msg_nxdomain_cname, "message_nxdomain_cname.wire");
    cache->update(msg_nxdomain_cname);

    msg_nxdomain_cname.makeResponse();

    Name a_example_org("a.example.org.");
    // The message should be cached
    EXPECT_TRUE(cache->lookup(a_example_org, RRType::A(), RRClass::IN(), msg_nxdomain_cname));

    EXPECT_EQ(msg_nxdomain_cname.getRcode().getCode(), Rcode::NXDOMAIN().getCode());
}

TEST_F(NegativeCacheTest, testNoerrorNodata){
    // NODATA/NOERROR response for MX type query of example.com
    Message msg_nodata(Message::PARSE);
    messageFromFile(msg_nodata, "message_nodata_with_soa.wire");
    cache->update(msg_nodata);

    msg_nodata.makeResponse();

    Name example_dot_com("example.com.");
    EXPECT_TRUE(cache->lookup(example_dot_com, RRType::MX(), RRClass::IN(), msg_nodata));

    RRsetIterator iter = msg_nodata.beginSection(Message::SECTION_AUTHORITY);
    RRsetPtr rrset_ptr = *iter;

    // The TTL should equal to the TTL of SOA record
    const RRTTL& nodata_ttl1 = rrset_ptr->getTTL();
    EXPECT_EQ(nodata_ttl1.getValue(), 86400);


    // Normal SOA response for example.com
    Message msg_example_com_soa(Message::PARSE);
    messageFromFile(msg_example_com_soa, "message_example_com_soa.wire");
    cache->update(msg_example_com_soa);

    msg_example_com_soa.makeResponse();
    Name soa_qname("example.com.");
    EXPECT_TRUE(cache->lookup(soa_qname, RRType::SOA(), RRClass::IN(), msg_example_com_soa));

    iter = msg_example_com_soa.beginSection(Message::SECTION_ANSWER);
    rrset_ptr = *iter;

    // The TTL should equal to the TTL of SOA record in answer section
    const RRTTL& soa_ttl = rrset_ptr->getTTL();
    EXPECT_EQ(soa_ttl.getValue(), 172800);

    // Query MX record of example.com again
    Message msg_nodata2(Message::PARSE);
    messageFromFile(msg_nodata2, "message_nodata_with_soa.wire");
    msg_nodata2.makeResponse();

    EXPECT_TRUE(cache->lookup(example_dot_com, RRType::MX(), RRClass::IN(), msg_nodata2));
    iter = msg_nodata2.beginSection(Message::SECTION_AUTHORITY);
    rrset_ptr = *iter;

    // The TTL should equal to the TTL of negative response SOA record
    const RRTTL& nodata_ttl2 = rrset_ptr->getTTL();
    EXPECT_EQ(nodata_ttl2.getValue(), 86400);
}

TEST_F(NegativeCacheTest, testReferralResponse){
    // CNAME exist, but it points to out of zone data, so the server give some reference data
    Message msg_cname_referral(Message::PARSE);
    messageFromFile(msg_cname_referral, "message_cname_referral.wire");
    cache->update(msg_cname_referral);

    msg_cname_referral.makeResponse();

    Name x_example_org("x.example.org.");
    EXPECT_TRUE(cache->lookup(x_example_org, RRType::A(), RRClass::IN(), msg_cname_referral));

    // The Rcode should be NOERROR
    EXPECT_EQ(msg_cname_referral.getRcode().getCode(), Rcode::NOERROR().getCode());
}

}

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

#include <iostream>
#include <exceptions/exceptions.h>
#include <dhcp/libdhcp++.h>
#include <dhcp/dhcp6.h>

#include "perf_pkt6.h"
#include "pkt_transform.h"

using namespace std;
using namespace isc;
using namespace dhcp;

namespace isc {
namespace perfdhcp {

PerfPkt6::PerfPkt6(const uint8_t* buf,
                   size_t len,
                   size_t transid_offset,
                   uint32_t transid) :
    Pkt6(buf, len, Pkt6::UDP),
    transid_offset_(transid_offset) {
    transid_ = transid;
}

bool
PerfPkt6::rawPack() {
    return (PktTransform::pack(dhcp::Option::V6,
                               data_,
                               options_,
                               transid_offset_,
                               transid_,
                               bufferOut_));
}

bool
PerfPkt6::rawUnpack() {
    return (PktTransform::unpack(dhcp::Option::V6,
                                 data_,
                                 options_,
                                 transid_offset_,
                                 transid_));
}

} // namespace perfdhcp
} // namespace isc

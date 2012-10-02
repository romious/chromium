// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Utility functions that act on syncable::MutableEntry to resolve conflicts.

#ifndef SYNC_ENGINE_CONFLICT_UTIL_H_
#define SYNC_ENGINE_CONFLICT_UTIL_H_

namespace syncer {

namespace syncable {
class MutableEntry;
}

// Marks the item as no longer requiring sync, allowing the server's version
// to 'win' during the next update application step.
void IgnoreLocalChanges(syncable::MutableEntry* entry);

// Marks the item as no longer requiring update from server data.  This will
// cause the item to be committed to the server, overwriting the server's
// version.
void OverwriteServerChanges(syncable::MutableEntry* entry);

// The local and server versions are identical, so unset the bits that put them
// into a conflicting state.
void IgnoreConflict(syncable::MutableEntry *trans);

}  // namespace syncer

#endif  // SYNC_ENGINE_CONFLICT_UTIL_H_

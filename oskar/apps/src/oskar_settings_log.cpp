/*
 * Copyright (c) 2012-2017, The University of Oxford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Oxford nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "apps/oskar_settings_log.h"
#include "settings/oskar_SettingsNode.h"
#include <cstring>

using namespace std;

static void oskar_settings_log_private(const oskar::SettingsTree* s,
        oskar_Log* log, const oskar::SettingsNode* node, int depth)
{
    if (!s->dependencies_satisfied(node->key())) return;

    if (node->priority() > 0 || node->value_or_child_set() ||
            node->is_required())
    {
        const char* label = node->label();
        const char* value = node->value();
        const int len = strlen(value);
        if (len == 0)
            oskar_log_message(log, 'M', depth, label);
        else if (len > 35)
            oskar_log_message(log, 'M', depth, "%s: %s", label, value);
        else
            oskar_log_value(log, 'M', depth, label, "%s", value);

        for (int i = 0; i < node->num_children(); ++i)
            oskar_settings_log_private(s, log, node->child(i), depth + 1);
    }
}

void oskar_settings_log(const oskar::SettingsTree* s, oskar_Log* log)
{
    const oskar::SettingsNode* node = s->root_node();
    for (int i = 0; i < node->num_children(); ++i)
        oskar_settings_log_private(s, log, node->child(i), 0);
    for (int i = 0; i < s->num_failed_keys(); ++i)
        oskar_log_warning(log, "Ignoring '%s'='%s'",
                s->failed_key(i), s->failed_key_value(i));
}

#include "pch.h"
#include "CmsManager.h"
#include "../ServerCore/JsonUtils.h"
{%- for entry in entries %}
#include "{{ entry }}Desc.h"
{%- endfor %}

void CmsManager::Init(const string& directory)
{
{%- for entry in entries %}
    CmsCached<{{ entry }}Desc>::Load(directory ,"{{ entry }}");
{%- endfor %}
}

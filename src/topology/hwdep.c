/*
  Copyright(c) 2018 Intel Corporation
  All rights reserved.

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  Authors: Pierre-Louis Bossart <pierre-louis.bossart@linux.intel.com>

*/

#include "list.h"
#include "tplg_local.h"

/* Parse HWdep */
int tplg_parse_hwdep(snd_tplg_t *tplg,
	snd_config_t *cfg, void *private ATTRIBUTE_UNUSED)
{
	struct snd_soc_tplg_hwdep *be;
	struct tplg_elem *elem;
	snd_config_iterator_t i, next;
	snd_config_t *n;
	const char *id, *val = NULL;

	elem = tplg_elem_new_common(tplg, cfg, NULL, SND_TPLG_TYPE_HWDEP);
	if (!elem)
		return -ENOMEM;

	be = elem->hwdep;
	be->size = elem->size;
	elem_copy_text(be->name, elem->id, SNDRV_CTL_ELEM_ID_NAME_MAXLEN);

	tplg_dbg(" HWdep: %s\n", elem->id);

	snd_config_for_each(i, next, cfg) {
		n = snd_config_iterator_entry(i);
		if (snd_config_get_id(n, &id) < 0)
			continue;

		/* skip comments */
		if (strcmp(id, "comment") == 0)
			continue;
		if (id[0] == '#')
			continue;

		if (strcmp(id, "bytes_max") == 0) {
			if (snd_config_get_string(n, &val) < 0)
				return -EINVAL;

			be->bytes_max = atoi(val);
			tplg_dbg("\t%s: %d\n", id, be->bytes_max);
			continue;
		}

		if (strcmp(id, "iface") == 0) {
			if (snd_config_get_string(n, &val) < 0)
				return -EINVAL;

			be->iface = atoi(val);
			tplg_dbg("\t%s: %d\n", id, be->iface);
			continue;
		}

	}

	return 0;
}

int tplg_add_hwdep(snd_tplg_t *tplg, struct snd_tplg_hwdep_template *hwdep,
	struct tplg_elem **e)
{
	struct snd_soc_tplg_hwdep *be;
	struct tplg_elem *elem;

	tplg_dbg(" HWdep: %s\n", hwdep->name);

	elem = tplg_elem_new_common(tplg, NULL, hwdep->name,
		SND_TPLG_TYPE_HWDEP);
	if (!elem)
		return -ENOMEM;

	be = elem->hwdep;
	be->size = elem->size;
	be->iface = hwdep->iface;
	be->bytes_max = hwdep->bytes_max;

	if (e)
		*e = elem;
	return 0;
}

int tplg_add_hwdep_object(snd_tplg_t *tplg, snd_tplg_obj_template_t *t)
{
	return tplg_add_hwdep(tplg, t->hwdep, NULL);
}

/* check referenced private data for a hwdep */
static int tplg_build_hwdep(snd_tplg_t *tplg, struct tplg_elem *elem)
{
	struct tplg_ref *ref;
	struct list_head *base, *pos;
	int err;

	base = &elem->ref_list;

	list_for_each(pos, base) {

		ref = list_entry(pos, struct tplg_ref, list);
		if (ref->elem)
			continue;

		 if (ref->type == SND_TPLG_TYPE_HWDEP) {
			err = tplg_copy_data(tplg, elem, ref);
			if (err < 0)
				return err;
		}
	}

	return 0;
}

int tplg_build_hwdeps(snd_tplg_t *tplg)
{
	struct list_head *base, *pos;
	struct tplg_elem *elem;
	int err = 0;

	base = &tplg->hwdep_list;
	list_for_each(pos, base) {

		elem = list_entry(pos, struct tplg_elem, list);
		err = tplg_build_hwdep(tplg, elem);
		if (err < 0)
			return err;

		/* add hwdep to manifest */
		tplg->manifest.hwdep_elems++;
	}

	return 0;
}

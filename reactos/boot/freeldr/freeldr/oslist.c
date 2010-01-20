/*
 *  FreeLoader
 *  Copyright (C) 1998-2003  Brian Palmer  <brianp@sginet.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <freeldr.h>

BOOLEAN InitOperatingSystemList(PCSTR **SectionNamesPointer, PCSTR **DisplayNamesPointer, ULONG* OperatingSystemCountPointer)
{
	ULONG		Idx;
	ULONG		CurrentOperatingSystemIndex;
	CHAR	SettingName[260];
	CHAR	SettingValue[260];
	ULONG		OperatingSystemCount;
	ULONG_PTR	SectionId;
	ULONG		SectionSettingCount;
	PCHAR	*OperatingSystemSectionNames;
	PCHAR	*OperatingSystemDisplayNames;

	//
	// Open the [FreeLoader] section
	//
	if (!IniOpenSection("Operating Systems", &SectionId))
	{
		UiMessageBox("Section [Operating Systems] not found in freeldr.ini.");
		return FALSE;
	}

	SectionSettingCount = IniGetNumSectionItems(SectionId);
	OperatingSystemCount = CountOperatingSystems(SectionId);

	//
	// Allocate memory to hold operating system lists
	//
	if (!AllocateListMemory(&OperatingSystemSectionNames, &OperatingSystemDisplayNames, OperatingSystemCount))
	{
		return FALSE;
	}

	//
	// Now loop through and read the operating system section and display names
	//
	CurrentOperatingSystemIndex = 0;
	for (Idx=0; Idx<SectionSettingCount; Idx++)
	{
		IniReadSettingByNumber(SectionId, Idx, SettingName, sizeof(SettingName), SettingValue, sizeof(SettingValue));

		// Copy the section name
		strcpy(OperatingSystemSectionNames[CurrentOperatingSystemIndex], SettingName);

		// Copy the display name
		RemoveQuotes(SettingValue);
		strcpy(OperatingSystemDisplayNames[CurrentOperatingSystemIndex], SettingValue);

		CurrentOperatingSystemIndex++;
	}

	*OperatingSystemCountPointer = OperatingSystemCount;
	*SectionNamesPointer = (PCSTR*)OperatingSystemSectionNames;
	*DisplayNamesPointer = (PCSTR*)OperatingSystemDisplayNames;

	return TRUE;
}

ULONG CountOperatingSystems(ULONG SectionId)
{
	return IniGetNumSectionItems(SectionId);
}

BOOLEAN AllocateListMemory(PCHAR **SectionNamesPointer, PCHAR **DisplayNamesPointer, ULONG OperatingSystemCount)
{
	ULONG		Idx;
	PCHAR	*OperatingSystemSectionNames = NULL;
	PCHAR	*OperatingSystemDisplayNames = NULL;

	//
	// Allocate memory to hold operating system list arrays
	//
	OperatingSystemSectionNames = MmHeapAlloc( sizeof(PCHAR) * OperatingSystemCount);
	OperatingSystemDisplayNames = MmHeapAlloc( sizeof(PCHAR) * OperatingSystemCount);

	//
	// If either allocation failed then return FALSE
	//
	if ( (OperatingSystemSectionNames == NULL) || (OperatingSystemDisplayNames == NULL) )
	{
		if (OperatingSystemSectionNames != NULL)
		{
			MmHeapFree(OperatingSystemSectionNames);
		}

		if (OperatingSystemDisplayNames != NULL)
		{
			MmHeapFree(OperatingSystemDisplayNames);
		}

		return FALSE;
	}

	//
	// Clear our newly allocated memory
	//
	memset(OperatingSystemSectionNames, 0, sizeof(PCHAR) * OperatingSystemCount);
	memset(OperatingSystemDisplayNames, 0, sizeof(PCHAR) * OperatingSystemCount);

	//
	// Loop through each array element and allocate it's string memory
	//
	for (Idx=0; Idx<OperatingSystemCount; Idx++)
	{
		OperatingSystemSectionNames[Idx] = MmHeapAlloc(80);
		OperatingSystemDisplayNames[Idx] = MmHeapAlloc(80);

		//
		// If it failed then jump to the cleanup code
		//
		if ( (OperatingSystemSectionNames[Idx] == NULL) || (OperatingSystemDisplayNames[Idx] == NULL))
		{
			goto AllocateListMemoryFailed;
		}
	}

	*SectionNamesPointer = OperatingSystemSectionNames;
	*DisplayNamesPointer = OperatingSystemDisplayNames;

	return TRUE;

AllocateListMemoryFailed:

	//
	// Loop through each array element and free it's string memory
	//
	for (Idx=0; Idx<OperatingSystemCount; Idx++)
	{
		if (OperatingSystemSectionNames[Idx] != NULL)
		{
			MmHeapFree(OperatingSystemSectionNames[Idx]);
		}

		if (OperatingSystemDisplayNames[Idx] != NULL)
		{
			MmHeapFree(OperatingSystemDisplayNames[Idx]);
		}
	}

	//
	// Free operating system list arrays
	//
	MmHeapFree(OperatingSystemSectionNames);
	MmHeapFree(OperatingSystemDisplayNames);

	return FALSE;
}

BOOLEAN RemoveQuotes(PCHAR QuotedString)
{
	CHAR	TempString[200];
	PCHAR p;
	PSTR Start;

	//
	// Skip spaces up to "
	//
	p = QuotedString;
	while (*p == ' ' || *p == '"')
		p++;
	Start = p;

	//
	// Go up to next "
	//
	while (*p != '"' && *p != ANSI_NULL)
		p++;
	*p = ANSI_NULL;

	//
	// Copy result
	//
	strcpy(TempString, Start);
	strcpy(QuotedString, TempString);

	return TRUE;
}

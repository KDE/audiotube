-- SPDX-FileCopyrightText: 2026 Jonah Brüchert <jbb@kaidan.im>
--
-- SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

alter table songs add column artist_id Text;
alter table songs add column album_id Text;

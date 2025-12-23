"use server";

import React from "react";

export type PageData = {
  title: string;
  url: string;
};

export async function fetchPagesBoolean(text: string): Promise<PageData[]> {
  const result = await fetch(
    encodeURI("http://158.160.72.26:8080/search/boolean?text=" + text),
  );
  console.log(result.status, result.statusText);
  if (result.status != 200) return [];
  try {
    return await result.json();
  } catch (e) {
    console.log("ERROR:", e);
    return [];
  }
}

export async function fetchPagesQuote(text: string): Promise<PageData[]> {
  const result = await fetch(
    encodeURI("http://158.160.72.26:8080/search/quote?text=" + text),
  );
  console.log(result.status, result.statusText);
  if (result.status != 200) return [];
  try {
    return await result.json();
  } catch (e) {
    console.log("ERROR:", e);
    return [];
  }
}

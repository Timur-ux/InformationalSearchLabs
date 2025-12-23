"use client";
import React from "react";
import {
  PageData,
  fetchPagesBoolean,
  fetchPagesQuote,
} from "./features/fetchPages";

export default function Home() {
  const [text, setText] = React.useState("");
  const [counter, setCounter] = React.useState<number | null>(null);
  const [responses, setResponses] = React.useState<PageData[]>([]);
  const [searchType, setSearchType] = React.useState<"boolean" | "quote">(
    "boolean",
  );
  const [wait, setWait] = React.useState<boolean | null>(null);
  return (
    <div>
      <div>
        <div className="mx-auto justify-between flex bg-gray-900 p-10 rounded-xl">
          <button
            className="p-5 bg-blue-500 rounded-xl mr-10"
            onClick={async () => {
              if (text.length == 0) {
                alert("Empty request not allowed");
                return;
              }
							setWait(true);
							setCounter(null);
              const pages: PageData[] = [];
              if (searchType == "boolean") {
                const newPages = await fetchPagesBoolean(text);
                pages.push(...newPages);
              } else if (searchType == "quote") {
                const newPages = await fetchPagesQuote(text);
                pages.push(...newPages);
              }
							setWait(false);
              setCounter(pages.length);
              setResponses(pages);
            }}
          >
            search
          </button>
          <input
            className="w-full bg-white rounded-xl text-black p-5"
            placeholder="Input request here"
            onChange={(e) => setText(e.target.value)}
            name="text"
          />
        </div>
        <div className="flex justify-center">
          <div className="flex">
            <span className="mx-2">Булевский поиск</span>
            <input
              name="isBoolean"
              type="radio"
              onChange={() => {}}
              checked={searchType == "boolean"}
              onClick={() => setSearchType("boolean")}
            />
          </div>
          <div className="flex">
            <span className="mx-2">Цитатный поиск</span>
            <input
              name="isQuote"
              type="radio"
              onChange={() => {}}
              checked={searchType == "quote"}
              onClick={() => setSearchType("quote")}
            />
          </div>
        </div>
      </div>
      {wait != null && wait && <p>Идет поиск...</p>}
      {counter != null && <p> Найдено страниц: {counter}</p>}
      <ul className="w-full py-5">
        {responses.map((response: PageData, index: number) => (
          <li className="py-5" key={index}>
            <a href={response.url}>
              <p>{response.title}</p>
              <p>{response.url}</p>
            </a>
          </li>
        ))}
      </ul>
    </div>
  );
}

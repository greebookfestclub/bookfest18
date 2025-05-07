= ミニマリストアプローチで Next.js ブログを作ってみよう

こんにちは。@<tt>{@cakegaly} です。普段は、グリーグループのエンジニアとして、DX 事業におけるプロダクト開発を担当しています。
ここでは、「ミニマリストアプローチ！公式の Next.js / Vercel 製だけでどこまでできる！？」をテーマに作成した Next.js ブログテンプレートを紹介します。

== はじめに

Next.js は、Vercel 社が開発する React ベースの Web フレームワークです。
2024年の State of JS @<fn>{state-of-js-2024} でも上位にランクインしており、
2025年現在では、React の公式サイトでも「おすすめのフレームワーク」として紹介されています。
SSR / SG (SSG) / ISR / API Routes / Middleware / RSC などをサポートしており、オールインワンな Web フレームワークへと進化しています。

一方で、「できることが多すぎて、どこから手をつけてよいかわからない」といった声もよく耳にします。
筆者自身も、「Next.js を使ってサクッとシンプルなサイトを作りたいだけなのに、気がつけば依存や設定ファイルだらけに！」という経験があります。

そこで、表題の通り、「ミニマリストアプローチで Next.js ブログを作る」をテーマに、
「まずはシンプルに Next.js でブログを作ってみたい！」を実現するテンプレートを作成しました。
以降では、その設計方針、構成、実装方法、カスタマイズ手順を解説します。

== next-minimal-blog を動かしてみよう

=== next-minimal-blog の概要

本稿の題材となるテンプレート next-minimal-blog について紹介します。
next-minimal-blog は、Next.js 15 (App Router) + MDX + shadcn/ui + Tailwind CSS を用いて作成したブログテンプレートです。

  * 公式ツール中心の構成 (Next.js, Vercel 製ツール)
  * 最小限の依存ライブラリ (@<code>{shadcn/ui}, @<code>{rehype-pretty-code} など)
  * MDX による記事コンテンツ管理
  * RSS / サイトマップの自動生成
  * OGP fetch によるリンクプレビュー
  * Tailwind CSS ベースのカスタマイズ可能なスタイル
  * ダークモード対応、ページネーション機能 など

構成や機能のシンプルさ、柔軟さのバランスを重視しています。
実際に、筆者の個人ブログ @<fn>{cakegaly-personal-blog} も、このテンプレートで構築して運用しています。
なお、テンプレートは、筆者の GitHub リポジトリ @<fn>{cakegaly-next-minimal-blog} で公開しています。
以降の内容とあわせて、ぜひお手元で動かしてみてください。

=== ディレクトリ構成

next-minimal-blog では、プロジェクト全体の見通しやすさを意識して、機能ごとにディレクトリを整理しています。

//list[src-directory-composition][src/ 以下のディレクトリ構成][bash]{
src/
├── actions/        # Server Actions (例: OGP メタデータの取得)
├── app/            # App Router のページ群 + RSS, Sitemap, Robots 出力
├── components/     # UI コンポーネント群
├── content/        # 記事コンテンツ (MDX)
├── config/         # サイト設定 / タグ定義などの定数
├── lib/            # ヘルパー関数 (MDX, OGP, ページネーション)
├── styles/         # Tailwind CSS / MDX 向けのスタイル定義
└── types/          # 型定義 (Frontmatter, 記事, OGP など)
//}

散らかりがちな @<code>{components/} 以下は、用途別に分類してディレクトリを分けています。
@<code>{shadcn/ui} コンポーネントは、デフォルトの格納先である @<code>{components/ui/} ではなく、@<code>{components/shadcn-ui/} にリネームして管理しています。
こうすることで、自作コンポーネントとの棲み分けが明確になり、プロジェクトの構造がよりわかりやすくなります。

//list[components-directory-composition][components/ 以下のディレクトリ構成][bash]{
components/
├── content/      # MDX 表示に関するカスタムコンポーネント
├── icons/        # アイコン素材 (Lucide Icon, SVG)
├── layout/       # レイアウト関連 (例: SiteHeader)
├── shared/       # 汎用 UI コンポーネント (例: Callout)
└── shadcn-ui/    # shadcn/ui コンポーネント群
//}

=== Tailwind CSS, shadcn/ui, MDX について

このテンプレートでは、スタイルや UI の構築において以下の技術を採用しています。

  * Tailwind CSS: ユーティリティファーストな CSS フレームワーク
  * shadcn/ui: Tailwind CSS + Radix UI ベースの再利用可能な UI コンポーネント集
  * MDX: Markdown + JSX による記事コンテンツの記述

Tailwind CSS は、テーマカラーやフォント、レスポンシブなどを自由にカスタマイズ可能で、@<code>{tailwind.config.ts} に設定を集約しています。
shadcn/ui は、React + Tailwind による「ちょうどよく使える UI コンポーネント集」で、ダークモードやアクセシビリティへの配慮がしっかりされています。
MDX は、Markdown に JSX を埋め込めるフォーマットであり、ブログ記事を記述するのに最適です。
本テンプレートでは、MDX ファイルを @<code>{src/content/} に保存し、fs + gray-matter によって読み込む構成を採用しています。

== MDX コンテンツの管理

=== ヘルパー関数を自作する

記事コンテンツは、すべて @<code>{.mdx} ファイルとして @<code>{src/content/blog/} 以下に保存します。
ブログ記事を表示するページでは、@<code>{fs} モジュールと @<code>{gray-matter} により記事コンテンツを静的に読み込んでいます。
これらを実現するヘルパー関数は、@<code>{lib/mdx.ts} にまとめています。

//list[lib-mdx][lib/mdx.ts に作成した MDX ファイル操作のためのヘルパー関数][TypeScript]{
import fs from 'fs';
import matter from 'gray-matter';
import path from 'path';

import { TechIcons } from '@/components/icons';
import { Frontmatter, MDXData } from '@/types/mdx';

const blogDir = path.join(process.cwd(), 'src', 'content', 'blog');

export type BlogPost = MDXData<{
  thumbnail?: string;
  tags?: string[];
  icon?: keyof typeof TechIcons;
}>;

export async function getAllBlogPosts(): Promise<BlogPost[]> {
  const posts = await getMDXData(blogDir);
  return posts.sort(
    (a, b) =>
      new Date(b.metadata.date).getTime() - new Date(a.metadata.date).getTime()
  );
}

export async function getBlogPostBySlug(slug: string) {
  return getBlogPost((post) => post.slug === slug);
}

async function getBlogPost(
  predicate: (post: BlogPost) => boolean
): Promise<BlogPost | undefined> {
  const posts = await getAllBlogPosts();
  return posts.find(predicate);
}

async function getMDXData<T>(dir: string): Promise<MDXData<T>[]> {
  const files = await getMDXFiles(dir);
  return Promise.all(files.map((file) => readMDXFile<T>(path.join(dir, file))));
}

async function getMDXFiles(dir: string): Promise<string[]> {
  return (await fs.promises.readdir(dir)).filter(
    (file) => path.extname(file) === '.mdx'
  );
}

async function readMDXFile<T>(filePath: string): Promise<MDXData<T>> {
  const rawContent = await fs.promises.readFile(filePath, 'utf-8');

  const { data, content } = matter(rawContent);

  return {
    metadata: data as Frontmatter<T>,
    slug: path.basename(filePath, path.extname(filePath)),
    rawContent: content,
  };
}
//}

ファイルシステムを直接読み取り、Frontmatter と本文を抽出するシンプルな構成です。
これにより、MDX コンテンツ管理のための外部ツール (例: Contentlayer)、CMS を使わずに、完全な SSG (静的生成) を実現しています。

MDX コンテンツ本体の解析は、公式ライブラリである @<code>{@mdx-js/mdx} で実現しています。
コードブロックのシンタックスハイライトについては、公式の推奨構成である @<code>{rehype-pretty-code} を利用しています。

//list[custom-mdx][components/content/custom-mdx.tsx][tsx]{
import { evaluate, type EvaluateOptions } from '@mdx-js/mdx';
import * as React from 'react';
import * as runtime from 'react/jsx-runtime';
import rehypePrettyCode from 'rehype-pretty-code';
import remarkGfm from 'remark-gfm';

import { components } from '@/components/content/mdx-components';

interface CustomMDXProps {
  source: string;
  additionalComponents?: Record<string, React.ComponentType<any>>;
}

const rehypePrettyCodeOptions = {
  theme: ellyTheme,
  keepBackground: true,
  defaultLang: 'plaintext',
};

export async function CustomMDX({
  source,
  additionalComponents,
}: CustomMDXProps) {
  try {
    const options: EvaluateOptions = {
      ...runtime,
      remarkPlugins: [remarkGfm],
      rehypePlugins: [[rehypePrettyCode, rehypePrettyCodeOptions]],
    };

    const { default: MDXContent } = await evaluate(source, options);

    const mergedComponents = {
      ...components,
      ...(additionalComponents || {}),
    };

    return <MDXContent components={mergedComponents} />;
  } catch (error) {
    console.error('Error rendering MDX:', error);
    return (
      {/* ... */}
    );
  }
}
//}

=== カスタムコンポーネントと連携する

MDX 内で @<code>{<Callout>}, @<code>{<LinkPreview>} などの React コンポーネントを使えるようにするため、
@<code>{components/content/mdx-components.tsx} に、コンポーネントのマッピングを定義します。
ここにスタイル定義を集約しているため、Tailwind Typography (@<code>{prose}) は導入せず、すべてのスタイルをコンポーネント側で制御できるようにしています。

//list[mdx-components][components/content/mdx-components.tsx][tsx]{
export const components = {
  h2: (props) => <h2 className="mt-12 text-2xl font-bold" {...props} />,
  a: (props) => <a className="text-primary underline" {...props} />,
  pre: (props) => <pre className="bg-muted/30 rounded-lg p-4" {...props} />,
  Callout,
  LinkPreview,
  // ...
};
//}

== OGP リンクプレビューの実装

記事内に登場する URL に対して OGP メタデータを取得し、リンクカード形式で表示する機能です。
Zenn や Qiita のような「リンクを貼るだけでカード表示される」体験を、MDX だけで実現することができます。

=== Server Action を使った OGP fetch 処理

リンク先の OGP メタデータは、Server Action を使って取得します。
以下のように @<code>{fetch()} で HTML を取得し、正規表現で必要なメタタグを抽出します。

//list[fetch-og-metadata][actions/fetch-og-metadata.ts][TypeScript]{
'use server';

import { cache } from 'react';

interface OGData {
  title: string;
  description: string;
  image: string;
  url: string;
}

export const getOGData = cache(
  async (url: string): Promise<Partial<OGData>> => {
    try {
      const response = await fetch(url, {
        headers: {
          'User-Agent': 'bot',
        },
      });

      const html = await response.text();

      const getMetaContent = (property: string): string | undefined => {
        const regex = new RegExp(
          `<meta[^>]+(?:property|name)="${property}"[^>]+content="([^"]+)"`,
          'i'
        );
        return regex.exec(html)?.[1];
      };

      const titleMatch = /<title>(.*?)<\/title>/i.exec(html);

      return {
        title: getMetaContent('og:title') || titleMatch?.[1] || '',
        description:
          getMetaContent('og:description') ||
          getMetaContent('description') ||
          '',
        image: getMetaContent('og:image') || '',
        url,
      };
    } catch (error) {
      console.error('Error fetching OG data:', error);
      return { url };
    }
  }
);
//}

@<code>{cache()} を使うことで、同一の URL に対してはサーバー上でキャッシュされ、不要な再取得を防止しています。

=== LinkPreview コンポーネント

リンクカードは @<code>{LinkPreview} コンポーネントで表示されます。
このコンポーネントでは、@<code>{Suspense} と組み合わせて、非同期的にリンクカードを描画します。

//list[link-preview][components/content/link-preview.tsx][tsx]{
export function LinkPreview({ url }: { url: string }) {
  return (
    <Suspense fallback={<LinkCardSkeleton />}>
      <LinkCardContent url={url} />
    </Suspense>
  );
}
//}

非同期部分では、@<code>{getOGData(url)} を呼び出してメタ情報を取得し、画像・タイトル・説明を含む UI を描画します。

//list[link-card-content][components/content/link-card-content.tsx][tsx]{
export async function LinkCardContent({ url }: { url: string }) {
  const og = await getOGData(url);

  return (
    <a href={og.url} className="block rounded-lg border p-4 hover:bg-muted">
      {og.image && <img src={og.image} className="mb-2 rounded" />}
      <div className="text-sm text-muted-foreground">{og.url}</div>
      <div className="font-bold">{og.title}</div>
      <p className="line-clamp-2">{og.description}</p>
    </a>
  );
}
//}

MDX ファイル内で @<code>{<LinkPreview url="https://example.com" />} のように書くだけで、自動的にリンクカードを表示できます。

=== 内部リンクと外部リンクの切り替えロジック

ブログ内リンク (例: @<code>{/blog/hello}) には OGP 情報が存在しないため、外部リンクのみプレビュー対象とすることで無駄な fetch を防ぎます。

//list[link-preview-2][components/content/link-preview.tsx][tsx]{
function isInternalBlogLink(url: string): boolean {
  try {
    const urlObj = new URL(url);
    return urlObj.pathname.startsWith('/blog/');
  } catch {
    return url.startsWith('/blog/');
  }
}
//}

内部リンクの場合は @<code>{next/link} でページ遷移させるだけにとどめ、
外部リンクの場合のみ @<code>{getOGData()} を使ってカードを表示する仕組みです。
このようにして、ユーザー体験とパフォーマンスのバランスを保ちながら、MDX によるリンクプレビューを実現しています。

== RSS・サイトマップの実装

この章では、ブログサイトにおける RSS フィードとサイトマップの自動生成について解説します。
これらは検索エンジンや RSS リーダーといった外部サービスとの連携に重要な要素ですが、
next-minimal-blog では追加ライブラリなしに、Next.js の Route Handlers 機能だけで完結させています。

=== RSS (Atom) フィードを出力する

@<code>{app/rss.xml/route.ts} を用意して、全記事を対象に、Atom 準拠の RSS フィードを構築しています。

//list[rss-xml-route][app/rss.xml/route.ts][TypeScript]{
import { NextResponse } from 'next/server';
import { siteConfig } from '@/config/site';
import { getAllBlogPosts } from '@/lib/mdx';

export async function GET() {
  const baseUrl = process.env.NEXT_PUBLIC_APP_URL || siteConfig.url;
  const posts = await getAllBlogPosts();

  const rssXml = `
    <rss version="2.0" xmlns:atom="http://www.w3.org/2005/Atom">
      <channel>
        <title>${siteConfig.name}</title>
        <link>${baseUrl}</link>
        <description>${siteConfig.description}</description>
        <language>ja</language>
        <lastBuildDate>${new Date().toUTCString()}</lastBuildDate>
        <atom:link href="${baseUrl}/rss.xml" rel="self" type="application/rss+xml"/>
        ${posts
          .map((post) => {
            return `
              <item>
                <title><![CDATA[${post.metadata.title}]]></title>
                <link>${baseUrl}/blog/${post.slug}</link>
                <guid isPermaLink="true">${baseUrl}/blog/${post.slug}</guid>
                <pubDate>${new Date(post.metadata.date).toUTCString()}</pubDate>
                <description><![CDATA[${post.metadata.description || ''}]]></description>
                ${
                  post.metadata.tags
                    ? post.metadata.tags
                        .map((tag) => `<category>${tag}</category>`)
                        .join('')
                    : ''
                }
              </item>
            `;
          })
          .join('')}
      </channel>
    </rss>
  `.trim();

  return new NextResponse(rssXml, {
    headers: {
      'Content-Type': 'application/xml',
      'Cache-Control': 'public, max-age=3600, s-maxage=3600',
    },
  });
}
//}

記事一覧をループしながら @<code>{<item>} エレメントを組み立て、RSS として有効な構造を構築しています。キャッシュヘッダーを適切に設定しておくことで、クローラーへの対応も万全です。

=== Sitemap を自動生成する

@<code>{next-sitemap} などの外部パッケージを使わず、@<code>{app/sitemap.ts} により、@<code>{MetadataRoute.Sitemap} 型でサイトマップを出力させます。
これにより、最小構成のまま SEO に必要な要素を実装できます。

//list[sitemap][app/sitemap.ts][TypeScript]{
import { siteConfig } from '@/config/site';
import { getAllBlogPosts } from '@/lib/mdx';
import { MetadataRoute } from 'next';

export default async function sitemap(): Promise<MetadataRoute.Sitemap> {
  const baseUrl = process.env.NEXT_PUBLIC_APP_URL || siteConfig.url;
  const posts = await getAllBlogPosts();

  const blogEntries = posts.map((post) => ({
    url: `${baseUrl}/blog/${post.slug}`,
    lastModified: new Date(post.metadata.date),
    changeFrequency: 'weekly' as const,
    priority: 0.8,
  }));

  const staticPages = [
    {
      url: baseUrl,
      lastModified: new Date(),
      changeFrequency: 'daily' as const,
      priority: 1.0,
    },
  ];

  return [...staticPages, ...blogEntries];
}
//}

トップページや静的ページは @<code>{staticPages} として定義して、各ブログ記事は @<code>{blogEntries} としてループ処理で生成します。
これらを結合して返すことで、検索エンジンがクロールしやすいサイト構成が完成します。
このように、Next.js 標準機能だけで RSS・サイトマップの要件を満たすことができ、余計な依存の追加を避けつつ、実用的な構成が実現できます。

== ページネーションの仕組み

この章では、ブログ記事を複数ページに分けて表示する「ページネーション」の実装について解説します。
next-minimal-blog では、shadcn/ui などの複雑なコンポーネントに依存せず、自作関数だけでシンプルにページネーションを実現しています。

=== ページネーションのロジック

ページネーションは、表示対象の記事リスト (items)、現在のページ番号 (currentPage)、総ページ数 (totalPages) を算出すれば、構築できます。
以下のように、配列をスライスして取得するだけの関数を用意すれば、基本的なロジックは完成します。

//list[lib-pagination][lib/pagination.ts][TypeScript]{
export interface PaginationResult<T> {
  items: T[];
  currentPage: number;
  totalPages: number;
  totalItems: number;
}

export function paginateItems<T>(
  items: T[],
  page = 1,
  pageSize = 10
): PaginationResult<T> {
  const totalItems = items.length;
  const totalPages = Math.ceil(totalItems / pageSize);
  const currentPage = Math.max(1, Math.min(page, totalPages));

  const startIndex = (currentPage - 1) * pageSize;
  const endIndex = Math.min(startIndex + pageSize, totalItems);

  return {
    items: items.slice(startIndex, endIndex),
    currentPage,
    totalPages,
    totalItems,
  };
}
//}

この関数に全記事一覧とページ番号を渡すことで、対応する記事だけを抽出できます。

=== ページ番号と記事の対応関係

Next.js App Router では、@<code>{generateStaticParams} により、動的なパラメータによるルーティングを生成します。
ここで、ページ番号に対応したビルド時のルーティングを生成しておきます。

//list[generate-static-params][page.tsx][tsx]{
export async function generateStaticParams() {
  const posts = await getAllBlogPosts();
  const { totalPages } = paginateItems(posts, 1, 10);
  return Array.from({ length: totalPages }, (_, i) => ({ page: String(i + 1) }));
}
//}

URL の @<code>{page} パラメータを使って @<code>{paginateItems()} を呼び出せば、該当ページのコンテンツが表示されます。

=== shadcn/ui に頼らない実装例

shadcn/ui には @<code>{Pagination} コンポーネントも用意されていますが、
ブログのようにそこまでページ数が多くならない前提であれば、
hooks なし、カスタムなしのプレーンな UI 実装で十分です。

//list[shared-pagination][components/shared/pagination.tsx][tsx]{
export function SimplePagination({
  currentPage,
  totalPages,
}: {
  currentPage: number;
  totalPages: number;
}) {
  const prevPage = currentPage > 1 ? `/page/${currentPage - 1}` : null;
  const nextPage = currentPage < totalPages ? `/page/${currentPage + 1}` : null;

  return (
    <div className="mt-12 flex justify-center gap-4">
      {prevPage && (
        <a href={prevPage} className="text-sm underline hover:opacity-70">
          ← 前のページ
        </a>
      )}
      <span className="text-sm text-muted-foreground">
        {currentPage} / {totalPages}
      </span>
      {nextPage && (
        <a href={nextPage} className="text-sm underline hover:opacity-70">
          次のページ →
        </a>
      )}
    </div>
  );
}
//}

このように、hooks やコンポーネントに頼らず、関数ベースで設計することで柔軟なカスタマイズと可読性の高い実装が可能になります。

== UI の整え方とカスタマイズ

この章では、next-minimal-blog の見た目を整えるために行ったカスタマイズについて紹介します。
Tailwind CSS のテーマカラー、MDX コンテンツの表示方法、記事カードに表示するアイコンの仕組みなど、ミニマルかつ実用的な UI を実現する工夫を取り上げます。

=== Tailwind カスタマイズ (テーマカラー)

next-minimal-blog では、Tailwind CSS のカラーパレットをカスタマイズして、落ち着いたダークテーマをベースにしています。
@<code>{tailwind.config.ts} では @<code>{extend} を使って、以下のように @<code>{primary} や @<code>{accent} といった色を @<code>{oklch} 形式で定義しています。

//list[tailwid-config-ts][tailwind.config.ts][TypeScript]{
colors: {
  background: 'oklch(15% 0.02 250)',
  foreground: 'oklch(90% 0.01 250)',
  primary: 'oklch(70% 0.15 220)',
  secondary: 'oklch(65% 0.14 130)',
  accent: 'oklch(75% 0.12 80)',
  destructive: 'oklch(40% 0.25 20)',
}
//}

背景色・前景色だけでなく、ボタンやリンク、アラート表示などに使う色も統一することで、全体に一貫性のあるデザインを実現しています。
なお、Tailwind v4 からは `oklch()` による色指定が公式でも推奨されており、よりアクセシビリティを意識したテーマ設計が可能です。

=== Tailwind Typography vs mdx-components

MDX コンテンツのスタイルは、一般的には Tailwind Typography プラグイン (@<code>{@tailwindcss/typography}) を使うケースが多いです。
しかし、next-minimal-blog では @<code>{prose} クラスを使わず、
独自に @<code>{mdx-components.tsx} を用意してコンポーネントごとにスタイルを当てています。

//list[mdx-components-2][components/content/mdx-components.tsx][tsx]{
export const components = {
  h2: (props) => <h2 className="mt-12 text-2xl font-bold" {...props} />,
  a: (props) => <a className="text-primary underline" {...props} />,
  pre: (props) => <pre className="bg-muted/30 rounded-lg p-4" {...props} />,
  Callout,
  LinkPreview,
  // 他にもカスタムコンポーネントをここで定義
};
//}

この方法により、クラス名や見た目を完全に制御できるほか、Tailwind Typography に依存しないため、テーマの一貫性や構成のシンプルさも保ちやすくなります。
また、@<code>{rehype-pretty-code} を使ったシンタックスハイライトや、外部リンクのプレビューなどもこの仕組みに統合することで、MDX コンテンツ全体を柔軟に制御できます。

=== アイコンと icon frontmatter の活用

next-minimal-blog では、記事一覧で「その記事がどの技術テーマに関するものか」を一目でわかるように、アイキャッチ代わりのアイコンを表示しています。
これを実現するために、各 MDX ファイルの Frontmatter に @<code>{icon} を指定する仕組みを導入しました。

//list[mdx-sample][src/content/blog/nextjs-tips.mdx][mdx]{
---
title: Next.js のちょっと便利な使い方
date: 2025-03-01
icon: nextjs
---
//}

記事カードを表示するコンポーネントでは、@<code>{icon} に指定されたキーを元に、@<code>{lucide-react} や独自の SVG アイコンとマッピングさせています。

//list[components-icons][components/icons/index.tsx][tsx]{
export const iconMap = {
  nextjs: <NextJsIcon />,
  react: <ReactIcon />,
  firebase: <FirebaseIcon />,
};
//}

こうすることで、記事ごとにテーマに応じたビジュアルを追加でき、Zenn のように一覧画面で情報が視覚的に伝わる構成が実現できます。
なお、@<code>{lucide-react} を採用することで、軽量で統一感のあるアイコンセットが利用できるだけでなく、必要に応じて独自の SVG を差し込むことも容易です。

== より実用的にするための Tips

この章では、ブログテンプレートを実際の運用に近づけるための応用テクニックを紹介します。
Vercel でのデプロイ方法、ヘッドレス CMS（microCMS）との連携、OGP 画像の自動生成や画像の最適化など、実際の開発・運用で役立つ内容を取り上げます。

=== Vercel でサイトを公開する

Next.js と Vercel は非常に親和性が高く、今回のブログテンプレートもワンクリックで簡単にデプロイ可能です。
Vercel に GitHub リポジトリを連携し、プロジェクトをインポートするだけでビルドとホスティングが完了します。
なお、環境変数 @<code>{NEXT_PUBLIC_APP_URL} を設定しておくと、RSS や Sitemap の出力で使用される URL に反映されるため、公開後の表示崩れやリンクミスを防げます。
独自ドメインの接続や、プレビュー機能、分析ツール（Analytics）も Vercel 側で簡単に設定できます。

=== microCMS と連携する

microCMS をはじめとした「ヘッドレス CMS」と連携させれば、より柔軟にコンテンツを管理できるようになります。
next-minimal-blog を拡張する場合は、@<code>{lib/mdx.ts} を @<code>{lib/microcms.ts} に差し替えるだけで、microCMS との接続に切り替えることが可能です。

//list[lib-microcms][lib/microcms.ts][TypeScript]{
import { createClient } from 'microcms-js-sdk';

const client = createClient({
  serviceDomain: 'your-service',
  apiKey: process.env.MICROCMS_API_KEY!,
});

export async function getAllArticles() {
  const data = await client.get({ endpoint: 'articles' });
  return data.contents;
}
//}

記事詳細ページも、@<code>{slug} を元に API 経由で取得するように変更すれば、ビルド時の SSG にも対応できます。
CMS 側の API 制限 (例: レート制限) や OGP の表示条件に応じて、fetch に @<code>{retry} や @<code>{sleep} 処理を加えると安定性が向上します。

=== OGP 自動生成・画像最適化などの応用例

Vercel では動的な画像生成 (例: @<code>{/api/og}) を利用できますが、完全 SSG の構成では事前に OGP 画像を生成しておく必要があります。
next-minimal-blog では、ローカル環境で OGP 画像を生成するスクリプトを用意して、ビルド前に画像を静的に書き出す運用を採用できます。

//list[package-json][package.json][JSON]{
"scripts": {
  "generate:ogp": "tsx scripts/generate-og-images.ts"
}
//}

また、画像の軽量化についても @<code>{pnpm optimize-image} のようなスクリプトを追加し、WebP 化やリサイズを一括処理することで、ページ表示速度や SEO にも好影響を与えます。
画像の最適化には @<code>{sharp}, @<code>{squoosh}, @<code>{imagemin} などのツールが使われますが、
公式の image loader (@<code>{next/image}) をうまく活用することで自動化できます。

== おわりに

最後までお読みいただき、ありがとうございました！
「Next.js のベストプラクティスを学びながら、自分のブログを立ち上げてみたい」という方にとって、本書がひとつの道しるべになれば嬉しいです。
冒頭でも紹介しましたが、ここで紹介したテンプレートは、筆者の GitHub リポジトリ @<fn>{cakegaly-next-minimal-blog} で公開しています。
ぜひ Fork や Star して、あなただけのブログを構築してみてください！

== 巻末付録

本章では、本書の開発環境における補足事項や、登場した用語の簡単な解説をまとめています。
より快適に Next.js を扱うための ESLint / Prettier / VSCode 設定のヒントや、App Router 以降の Next.js で頻出する用語の整理に活用してください。

=== Next.js を快適に開発する設定 (ESLint, Prettier, VSCode)

next-minimal-blog では、VSCode で快適に開発するために、ESLint (Flat Config) によるコードチェック、Prettier によるコード整形、保存時に Lint + Format が走る VSCode 設定を GitHub リポジトリに含めています。
特に、ESLint は v9 対応の Flat Config に切り替え、以下のように @<code>{eslint.config.mjs} をルートに設置しています。

//list[eslint-config][eslint.config.mjs][JavaScript]{
import js from '@eslint/js';
import ts from '@typescript-eslint/eslint-plugin';
import tsParser from '@typescript-eslint/parser';
import prettier from 'eslint-config-prettier';

export default [
  {
    files: ['**/*.ts', '**/*.tsx'],
    languageOptions: {
      parser: tsParser,
      parserOptions: {
        ecmaVersion: 2022,
        sourceType: 'module',
        project: true,
      },
    },
    plugins: {
      '@typescript-eslint': ts,
    },
    rules: {
      // 独自ルールや import 順序設定なども追加可能
    },
  },
  prettier,
];
//}

Prettier は、@<code>{prettier.config.mjs} で Tailwind プラグインや import ソートを設定しています。

//list[prettier-config][prettier.config.mjs][JavaScript]{
export default {
  plugins: [
    'prettier-plugin-tailwindcss',
    '@ianvs/prettier-plugin-sort-imports',
  ],
  tailwindConfig: './tailwind.config.ts',
  importOrder: ['^react$', '^next', '^@/.*', '^[./]'],
  importOrderSeparation: true,
  importOrderSortSpecifiers: true,
};
//}

VSCode 側では、@<code>{.vscode/settings.json} に以下を設定して、保存時に自動で Lint + Format を走らせています。

//list[vscode-json-setting][.vscode/settings.json][JSON]{
{
  "editor.formatOnSave": true,
  "editor.codeActionsOnSave": {
    "source.fixAll.eslint": true
  },
  "eslint.experimental.useFlatConfig": true
}
//}

これにより、複数人で開発する際にも一貫性のあるフォーマットが維持でき、Git 上の差分も最小限になります。

//footnote[state-of-js-2024][https://stateofjs.com/]
//footnote[cakegaly-personal-blog][https://cakegaly.com]
//footnote[cakegaly-next-minimal-blog][https://github.com/cakegaly/next-minimal-blog]

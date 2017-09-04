import { Injectable } from '@angular/core';
import { BasePoint, Seer } from 'app/classes/seer';
import { Observable } from 'rxjs/Rx';
import { RxWebsocketSubject } from './connection/connection';

class Resource<T> {
  public count: number = 0;
  public resource: T;
  constructor(resource: T) {
    this.resource = resource;
  }
}

@Injectable()
export class SeerConnectionService {
  // private connections: Map<string, RxWebsocketSubject<object>> = new Map<string, RxWebsocketSubject<object>>();
  private connections: Map<string, Resource<Seer>> = new Map<string, Resource<Seer>>();
  // constructor() { }

  public connect(url: string): Seer {
    if (this.connections.has(url)) {
      const connection: Resource<Seer> = this.connections.get(url);
      connection.count++;
      return connection.resource;
    } else {
      const source: RxWebsocketSubject<BasePoint> = new RxWebsocketSubject<BasePoint>(url);
      const connection: Resource<Seer> = new Resource<Seer>(new Seer(source.publish().refCount()));
      this.connections.set(url, connection);
      return connection.resource;
    }
  }

  public disconnect(url: string): void {
    if (this.connections.has(url)) {
      const connection: Resource<Seer> = this.connections.get(url);
      connection.count--;
      if (connection.count === 0) {
        this.connections.delete(url);
      }
    } else {
      throw new Error(`Nothing to disconnect from on: ${url} , disconnect must have been called to many times`);
    }
  }

  // public send(url: string, message: any): void {
  //   if (this.connections.has(url) === false) {
  //     throw new Error(`No connection by the url: ${url} exists, please call connect first`);
  //   }
  //   this.connections.get(url).send(message);
  // }
}
